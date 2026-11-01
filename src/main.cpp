#include "Database.h"
#include "Repositories.h"
#include "Services.h"
#include <iostream>
#include <cstdlib>
#include <limits>
#include <stdexcept>
#include <regex>
#include <ctime>
namespace {bool validDateTime(const std::string& value) {
    if (!std::regex_match(value,
        std::regex(R"(^\d{4}-\d{2}-\d{2} \d{2}:\d{2}:\d{2}$)"))) {
        return false;
    }

    int year = std::stoi(value.substr(0, 4));
    int month = std::stoi(value.substr(5, 2));
    int day = std::stoi(value.substr(8, 2));
    int hour = std::stoi(value.substr(11, 2));
    int minute = std::stoi(value.substr(14, 2));
    int second = std::stoi(value.substr(17, 2));

    if (month < 1 || month > 12 || hour > 23 ||
        minute > 59 || second > 59) {
        return false;
    }

    const int daysInMonth[] = {
        31, 28, 31, 30, 31, 30,
        31, 31, 30, 31, 30, 31
    };

    bool leapYear = (year % 400 == 0) ||
                    (year % 4 == 0 && year % 100 != 0);

    int maxDay = daysInMonth[month - 1];
    if (month == 2 && leapYear) {
        maxDay = 29;
    }

    return day >= 1 && day <= maxDay;
}
std::string defaultDbPath() {
    if (const char* p = std::getenv("LMS_DB"); p && *p) return p;
    if (const char* h = std::getenv("HOME");   h && *h) return std::string(h) + "/.lms.db";
    return "lms.db";
}
std::string ask(const std::string& label) {
    std::cout << label;
    std::string v; std::getline(std::cin,v); return v;
}
int askInt(const std::string& label) {
    while (true) {
        std::string s = ask(label);
        std::size_t used = 0;

        try {
            int v = std::stoi(s, &used);

            if (used != s.size()) {
                std::cout << "Invalid input. Please enter a whole number.\n";
                continue;
            }

            return v;
        }
        catch (const std::invalid_argument&) {
            std::cout << "Invalid input. Please enter a whole number.\n";
        }
        catch (const std::out_of_range&) {
            std::cout << "Number is too large. Please enter a smaller number.\n";
        }
    }
}
double askDouble(const std::string& label) {
    std::string s=ask(label); std::size_t used=0;
    double v=std::stod(s,&used); if(used!=s.size()) throw std::invalid_argument("Enter a number"); return v;
}
Role chooseRole() {
    std::cout<<"1 Student  2 Lecturer  3 HoD  4 Administrator\n";
    switch(askInt("Role: ")) {
    case 1:return Role::Student; case 2:return Role::Lecturer;
    case 3:return Role::HoD; case 4:return Role::Administrator;
    default:throw std::invalid_argument("Invalid role");
    }
}
void menu(User& u, LMSRepository& repo) {
    std::cout<<"\nWelcome "<<u.username()<<" — "<<u.dashboard()<<"\n";
    bool done=false;
    while(!done) {
      std::cout<<"\n1 List assignments\n2 List submissions\n3 List users (admin demo)\n4 Submit work (Student)\n"
                 "5 Create assignment (Lecturer/Admin)\n6 Mark submission (Lecturer)\n"
                 "7 Moderate mark (HoD)\n8 Release approved mark (HoD)\n9 Grant extension (HoD/Admin)\n"
                 "10 View my released marks (Student)\n0 Logout\n";
      int c=askInt("Choice: ");
      try {
        if(c==0) done=true;
        else if(c==1) {
          auto a=repo.assignments();
          for(auto& x:a) std::cout<<x.id<<" | "<<x.title<<" | deadline "<<x.deadline<<" | max "<<x.maxMark<<"\n";
        } else if(c==2) {
          if(u.role()!=Role::Lecturer && u.role()!=Role::HoD && u.role()!=Role::Administrator) throw std::runtime_error("Permission denied");
          for(auto& s:repo.submissions()) std::cout<<s.id<<" | assignment "<<s.assignmentId<<" | student "<<s.studentId<<" | "<<s.status<<" | "<<s.filePath<<"\n";
        } else if(c==3) {
          if(u.role()!=Role::Administrator) throw std::runtime_error("Permission denied");
          for(auto& name:repo.users()) std::cout<<name<<"\n";
        } else if(c==4) {
          if(u.role()!=Role::Student) throw std::runtime_error("Only students may submit");
          int aid=askInt("Assignment ID: "); std::string path=ask("Path to submitted file: ");
          std::string at=ask("Submission timestamp (YYYY-MM-DD HH:MM:SS): ");
          std::string deadline=ask("Effective deadline (YYYY-MM-DD HH:MM:SS; include extension if granted): ");
          std::string status=(at<=deadline)?"Submitted":"Late";
          repo.submit(aid,u.id(),path,at,status);
          std::cout<<"Submission saved with status "<<status<<". Confirm deadline and penalty policy against SRS.\n";
        } else if(c==5) {
          if(u.role()!=Role::Lecturer && u.role()!=Role::Administrator) throw std::runtime_error("Permission denied");
          int course=askInt("Course ID (must already exist): "); std::string title=ask("Title: ");
          std::string desc=ask("Description: "); std::string deadline=ask("Deadline (YYYY-MM-DD HH:MM:SS): ");
          double max=askDouble("Maximum mark: "); if(max<=0) throw std::invalid_argument("Maximum mark must be positive");
          std::cout<<"Created assignment ID "<<repo.createAssignment(course,title,desc,deadline,max,u.id())<<"\n";
        } else if(c==6) {
          if(u.role()!=Role::Lecturer) throw std::runtime_error("Only lecturers may mark");
          int sid=askInt("Submission ID: "); double score=askDouble("Raw mark: ");
          std::string fb=ask("Feedback/reason: "); repo.saveMark(sid,u.id(),score,fb);
          std::cout<<"Mark saved and audit event recorded. Check score against assignment maximum before release.\n";
        } else if(c==7) {
          if(u.role()!=Role::HoD) throw std::runtime_error("Only HoD may moderate");
          int mid=askInt("Mark ID: "); std::string yes=ask("Approve? (y/n): ");
          std::string reason=ask("Reason: "); repo.moderate(mid,u.id(),yes=="y"||yes=="Y",reason);
          std::cout<<"Moderation decision saved.\n";
        } else if(c==8) {
          if(u.role()!=Role::HoD) throw std::runtime_error("Only HoD may release marks");
          int mid=askInt("Mark ID: "); std::string reason=ask("Release reason: "); repo.release(mid,u.id(),reason);
          std::cout<<"Approved mark released.\n";
        } else if(c==9) {
          if(u.role()!=Role::HoD && u.role()!=Role::Administrator) throw std::runtime_error("Permission denied");
          int assignmentId = askInt("Assignment ID: ");
int studentId = askInt("Student user ID: ");
std::string deadline;

do {
    deadline = ask("New deadline (YYYY-MM-DD HH:MM:SS): ");

    if (!validDateTime(deadline)) {
        std::cout << "Invalid date or time. Use YYYY-MM-DD HH:MM:SS.\n";
    }
} while (!validDateTime(deadline));

repo.grantExtension(assignmentId, studentId, deadline, u.id());
          std::cout<<"Extension saved.\n";
        } else if(c==10) {
          if(u.role()!=Role::Student) throw std::runtime_error("Only students may view their marks");
          for(auto& m:repo.marksForStudent(u.id())) std::cout<<"Mark "<<m.id<<": "<<m.score<<" feedback: "<<m.feedback<<"\n";
        } else std::cout<<"Unknown option.\n";
      } catch(const std::exception& e) { std::cerr<<"Operation failed: "<<e.what()<<"\n"; }
    }
}
}
int main() {
 try {
   Database db(defaultDbPath()); LMSRepository repo(db); repo.initialiseSchema(); UserRepository users(db); AuthService auth(users);
 std::cout<<"LMS Phase 4 prototype (C++17 + SQLite)\n";
   bool running=true;
   while(running) {
     std::cout<<"\n1 Register user  2 Login  0 Exit\n";
     try {
       int c=askInt("Choice: ");
       if(c==0) running=false;
       else if(c==1) {
         std::string name=ask("Username: "); std::string pass=ask("Password (10+ characters): ");
         Role role=chooseRole();
         int id=auth.registerUser(name,pass,role);
         std::cout<<"Created user ID "<<id<<". NOTE: registration is open for development; restrict it before deployment.\n";
       } else if(c==2) {
         std::string name=ask("Username: "); std::string pass=ask("Password: ");
         auto user=auth.login(name,pass);
         if(!user) std::cout<<"Invalid username or password.\n"; else menu(*user,repo);
       } else std::cout<<"Unknown option.\n";
     } catch(const std::exception& e) { std::cerr<<"Input/operation error: "<<e.what()<<"\n"; }
   }
 } catch(const std::exception& e) { std::cerr<<"Fatal startup error: "<<e.what()<<"\n"; return 1; }
 return 0;
}
