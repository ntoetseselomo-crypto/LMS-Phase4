#include "Repositories.h"
#include <sqlite3.h>
#include <stdexcept>
#include <sstream>

namespace {
class Statement {
public:
    Statement(sqlite3* db, const char* sql) : db_(db) {
        if (sqlite3_prepare_v2(db, sql, -1, &st_, nullptr) != SQLITE_OK)
            throw std::runtime_error(sqlite3_errmsg(db));
    }
    ~Statement() { if (st_) sqlite3_finalize(st_); }
    sqlite3_stmt* get() { return st_; }
    void done() {
        int rc = sqlite3_step(st_);
        if (rc != SQLITE_DONE) throw std::runtime_error(sqlite3_errmsg(db_));
    }
private:
    sqlite3* db_;
    sqlite3_stmt* st_{nullptr};
};

void text(sqlite3_stmt* s, int i, const std::string& v) {
    if (sqlite3_bind_text(s, i, v.c_str(), -1, SQLITE_TRANSIENT) != SQLITE_OK)
        throw std::runtime_error("Could not bind text parameter");
}
}

int UserRepository::create(const std::string& username, const std::string& passwordHash, Role role) {
    Statement s(db_.handle(), "INSERT INTO Users(username,password_hash,role) VALUES(?,?,?);");
    text(s.get(),1,username);
    text(s.get(),2,passwordHash);
    text(s.get(),3,roleToString(role));
    s.done();
    return static_cast<int>(sqlite3_last_insert_rowid(db_.handle()));
}

std::optional<UserRepository::LoginRow> UserRepository::findByUsername(const std::string& username) {
    Statement s(db_.handle(), "SELECT user_id,username,password_hash,role FROM Users WHERE username=?;");
    text(s.get(),1,username);
    int rc=sqlite3_step(s.get());
    if (rc==SQLITE_DONE) return std::nullopt;
    if (rc!=SQLITE_ROW) throw std::runtime_error(sqlite3_errmsg(db_.handle()));
    return LoginRow{sqlite3_column_int(s.get(),0),
      reinterpret_cast<const char*>(sqlite3_column_text(s.get(),1)),
      reinterpret_cast<const char*>(sqlite3_column_text(s.get(),2)),
      roleFromString(reinterpret_cast<const char*>(sqlite3_column_text(s.get(),3)))};
}

void LMSRepository::initialiseSchema() {
 db_.execute(R"SQL(
 CREATE TABLE IF NOT EXISTS Department(department_id INTEGER PRIMARY KEY, name TEXT NOT NULL UNIQUE);
 CREATE TABLE IF NOT EXISTS Users(
   user_id INTEGER PRIMARY KEY, username TEXT NOT NULL UNIQUE, password_hash TEXT NOT NULL,
   role TEXT NOT NULL CHECK(role IN ('Student','Lecturer','HoD','Administrator')),
   department_id INTEGER REFERENCES Department(department_id));
 CREATE TABLE IF NOT EXISTS Course(
   course_id INTEGER PRIMARY KEY, course_name TEXT NOT NULL, department_id INTEGER REFERENCES Department(department_id),
   lecturer_id INTEGER REFERENCES Users(user_id));
 CREATE TABLE IF NOT EXISTS Enrolment(
   student_id INTEGER NOT NULL REFERENCES Users(user_id), course_id INTEGER NOT NULL REFERENCES Course(course_id),
   PRIMARY KEY(student_id,course_id));
 CREATE TABLE IF NOT EXISTS Assignment(
   assignment_id INTEGER PRIMARY KEY, course_id INTEGER REFERENCES Course(course_id),
   title TEXT NOT NULL, description TEXT, deadline TEXT NOT NULL, max_mark REAL NOT NULL CHECK(max_mark>0),
   created_by INTEGER REFERENCES Users(user_id));
 CREATE TABLE IF NOT EXISTS Extension(
   extension_id INTEGER PRIMARY KEY, assignment_id INTEGER NOT NULL REFERENCES Assignment(assignment_id),
   student_id INTEGER NOT NULL REFERENCES Users(user_id), new_deadline TEXT NOT NULL,
   granted_by INTEGER NOT NULL REFERENCES Users(user_id), UNIQUE(assignment_id,student_id));
 CREATE TABLE IF NOT EXISTS Submission(
   submission_id INTEGER PRIMARY KEY, assignment_id INTEGER NOT NULL REFERENCES Assignment(assignment_id),
   student_id INTEGER NOT NULL REFERENCES Users(user_id), submitted_at TEXT NOT NULL,
   file_path TEXT NOT NULL, status TEXT NOT NULL, version INTEGER NOT NULL DEFAULT 1);
 CREATE TABLE IF NOT EXISTS Mark(
   mark_id INTEGER PRIMARY KEY, submission_id INTEGER NOT NULL UNIQUE REFERENCES Submission(submission_id),
   marked_by INTEGER NOT NULL REFERENCES Users(user_id), score REAL NOT NULL CHECK(score>=0),
   feedback TEXT, moderation_status TEXT NOT NULL DEFAULT 'Pending'
   CHECK(moderation_status IN ('Pending','Approved','ChangesRequested')), released INTEGER NOT NULL DEFAULT 0);
 CREATE TABLE IF NOT EXISTS AuditLog(
   log_id INTEGER PRIMARY KEY, mark_id INTEGER NOT NULL REFERENCES Mark(mark_id),
   changed_by INTEGER NOT NULL REFERENCES Users(user_id), old_mark REAL, new_mark REAL NOT NULL,
   changed_at TEXT NOT NULL DEFAULT (datetime('now')), reason TEXT NOT NULL);
 CREATE INDEX IF NOT EXISTS idx_submission_assignment ON Submission(assignment_id);
 CREATE INDEX IF NOT EXISTS idx_submission_student ON Submission(student_id);
 )SQL");
}

int LMSRepository::createAssignment(int courseId,const std::string& title,const std::string& description,
 const std::string& deadline,double maxMark,int creatorId) {
 Statement s(db_.handle(),"INSERT INTO Assignment(course_id,title,description,deadline,max_mark,created_by) VALUES(?,?,?,?,?,?);");
 sqlite3_bind_int(s.get(),1,courseId);
 text(s.get(),2,title);
 text(s.get(),3,description);
 text(s.get(),4,deadline);
 sqlite3_bind_double(s.get(),5,maxMark);
 sqlite3_bind_int(s.get(),6,creatorId);
 s.done();
 return static_cast<int>(sqlite3_last_insert_rowid(db_.handle()));
}

std::vector<Assignment> LMSRepository::assignments() {
 Statement s(db_.handle(),"SELECT assignment_id,title,deadline,max_mark FROM Assignment ORDER BY assignment_id;");
 std::vector<Assignment> out;
 int rc;
 while((rc=sqlite3_step(s.get()))==SQLITE_ROW)
  out.push_back({sqlite3_column_int(s.get(),0),reinterpret_cast<const char*>(sqlite3_column_text(s.get(),1)),
   reinterpret_cast<const char*>(sqlite3_column_text(s.get(),2)),sqlite3_column_double(s.get(),3)});
 if(rc!=SQLITE_DONE) throw std::runtime_error(sqlite3_errmsg(db_.handle()));
 return out;
}

void LMSRepository::submit(int assignmentId, int studentId, const std::string& filePath,
                           const std::string& submittedAt, const std::string& status) {
 // Keep a submission history; each resubmission creates another version row.
 Statement s(db_.handle(),"INSERT INTO Submission(assignment_id,student_id,submitted_at,file_path,status,version) "
 "VALUES(?,?,?,?,?,1 + (SELECT COUNT(*) FROM Submission WHERE assignment_id=? AND student_id=?));");
 sqlite3_bind_int(s.get(),1,assignmentId);
 sqlite3_bind_int(s.get(),2,studentId);
 text(s.get(),3,submittedAt);
 text(s.get(),4,filePath);
 text(s.get(),5,status);
 sqlite3_bind_int(s.get(),6,assignmentId);
 sqlite3_bind_int(s.get(),7,studentId);
 s.done();
}

std::vector<Submission> LMSRepository::submissions() {
 Statement s(db_.handle(),"SELECT submission_id,assignment_id,student_id,submitted_at,file_path,status FROM Submission ORDER BY submission_id;");
 std::vector<Submission> out;
 int rc;
 while((rc=sqlite3_step(s.get()))==SQLITE_ROW)
  out.push_back({sqlite3_column_int(s.get(),0),sqlite3_column_int(s.get(),1),sqlite3_column_int(s.get(),2),
   reinterpret_cast<const char*>(sqlite3_column_text(s.get(),3)),reinterpret_cast<const char*>(sqlite3_column_text(s.get(),4)),
   reinterpret_cast<const char*>(sqlite3_column_text(s.get(),5))});
 if(rc!=SQLITE_DONE) throw std::runtime_error(sqlite3_errmsg(db_.handle()));
 return out;
}

void LMSRepository::saveMark(int submissionId,int lecturerId,double score,const std::string& feedback) {
 // Transaction ensures the mark and its audit record are committed together.
 db_.execute("BEGIN IMMEDIATE;");
 try {
   Statement q(db_.handle(),"SELECT mark_id,score FROM Mark WHERE submission_id=?;");
   sqlite3_bind_int(q.get(),1,submissionId);
   int rc=sqlite3_step(q.get());
   if(rc==SQLITE_ROW) {
     int markId=sqlite3_column_int(q.get(),0);
     double old=sqlite3_column_double(q.get(),1);
     Statement u(db_.handle(),"UPDATE Mark SET marked_by=?,score=?,feedback=?,moderation_status='Pending',released=0 WHERE mark_id=?;");
     sqlite3_bind_int(u.get(),1,lecturerId);
     sqlite3_bind_double(u.get(),2,score);
     text(u.get(),3,feedback);
     sqlite3_bind_int(u.get(),4,markId);
     u.done();
     Statement a(db_.handle(),"INSERT INTO AuditLog(mark_id,changed_by,old_mark,new_mark,reason) VALUES(?,?,?,?,?);");
     sqlite3_bind_int(a.get(),1,markId);
     sqlite3_bind_int(a.get(),2,lecturerId);
     sqlite3_bind_double(a.get(),3,old);
     sqlite3_bind_double(a.get(),4,score);
     text(a.get(),5,"Lecturer updated mark: "+feedback);
     a.done();
   } else if(rc==SQLITE_DONE) {
     Statement i(db_.handle(),"INSERT INTO Mark(submission_id,marked_by,score,feedback) VALUES(?,?,?,?);");
     sqlite3_bind_int(i.get(),1,submissionId);
     sqlite3_bind_int(i.get(),2,lecturerId);
     sqlite3_bind_double(i.get(),3,score);
     text(i.get(),4,feedback);
     i.done();
     int markId=static_cast<int>(sqlite3_last_insert_rowid(db_.handle()));
     Statement a(db_.handle(),"INSERT INTO AuditLog(mark_id,changed_by,old_mark,new_mark,reason) VALUES(?,?,NULL,?,?);");
     sqlite3_bind_int(a.get(),1,markId);
     sqlite3_bind_int(a.get(),2,lecturerId);
     sqlite3_bind_double(a.get(),3,score);
     text(a.get(),4,"Initial mark: "+feedback);
     a.done();
   } else throw std::runtime_error(sqlite3_errmsg(db_.handle()));
   db_.execute("COMMIT;");
 } catch(...) {
   try { db_.execute("ROLLBACK;"); } catch(...) {}
   throw;
 }
}

void LMSRepository::moderate(int markId,int hodId,bool approve,const std::string& reason) {
 Statement q(db_.handle(),"SELECT score FROM Mark WHERE mark_id=?;");
 sqlite3_bind_int(q.get(),1,markId);
 if(sqlite3_step(q.get())!=SQLITE_ROW) throw std::runtime_error("Mark not found");
 double score = sqlite3_column_double(q.get(),0);

 Statement s(db_.handle(),"UPDATE Mark SET moderation_status=?,released=0 WHERE mark_id=?;");
 text(s.get(),1,approve?"Approved":"ChangesRequested");
 sqlite3_bind_int(s.get(),2,markId);
 s.done();
 if(sqlite3_changes(db_.handle())!=1) throw std::runtime_error("Mark not found");

 Statement a(db_.handle(),"INSERT INTO AuditLog(mark_id,changed_by,old_mark,new_mark,reason) VALUES(?,?,?,?,?);");
 sqlite3_bind_int(a.get(),1,markId);
 sqlite3_bind_int(a.get(),2,hodId);
 sqlite3_bind_double(a.get(),3,score);
 sqlite3_bind_double(a.get(),4,score);
 text(a.get(),5,std::string("Moderation ")+(approve?"Approved":"ChangesRequested")+": "+reason);
 a.done();
}

void LMSRepository::release(int markId,int hodId,const std::string& reason) {
 Statement q(db_.handle(),"SELECT score FROM Mark WHERE mark_id=? AND moderation_status='Approved';");
 sqlite3_bind_int(q.get(),1,markId);
 if(sqlite3_step(q.get())!=SQLITE_ROW) throw std::runtime_error("Cannot release: mark missing or not approved");
 double score = sqlite3_column_double(q.get(),0);

 Statement s(db_.handle(),"UPDATE Mark SET released=1 WHERE mark_id=? AND moderation_status='Approved';");
 sqlite3_bind_int(s.get(),1,markId);
 s.done();
 if(sqlite3_changes(db_.handle())!=1) throw std::runtime_error("Cannot release: mark missing or not approved");

 Statement a(db_.handle(),"INSERT INTO AuditLog(mark_id,changed_by,old_mark,new_mark,reason) VALUES(?,?,?,?,?);");
 sqlite3_bind_int(a.get(),1,markId);
 sqlite3_bind_int(a.get(),2,hodId);
 sqlite3_bind_double(a.get(),3,score);
 sqlite3_bind_double(a.get(),4,score);
 text(a.get(),5,std::string("Released: ")+reason);
 a.done();
}

void LMSRepository::grantExtension(int assignmentId,int studentId,const std::string& newDeadline,int grantedBy) {
 Statement s(db_.handle(),"INSERT INTO Extension(assignment_id,student_id,new_deadline,granted_by) VALUES(?,?,?,?) "
 "ON CONFLICT(assignment_id,student_id) DO UPDATE SET new_deadline=excluded.new_deadline,granted_by=excluded.granted_by;");
 sqlite3_bind_int(s.get(),1,assignmentId);
 sqlite3_bind_int(s.get(),2,studentId);
 text(s.get(),3,newDeadline);
 sqlite3_bind_int(s.get(),4,grantedBy);
 s.done();
}

std::vector<MarkRecord> LMSRepository::marksForStudent(int studentId) {
 Statement s(db_.handle(),"SELECT m.mark_id,m.submission_id,m.score,COALESCE(m.feedback,''),m.moderation_status,m.released "
 "FROM Mark m JOIN Submission s ON s.submission_id=m.submission_id WHERE s.student_id=? AND m.released=1 ORDER BY m.mark_id;");
 sqlite3_bind_int(s.get(),1,studentId);
 std::vector<MarkRecord> out;
 int rc;
 while((rc=sqlite3_step(s.get()))==SQLITE_ROW)
  out.push_back({sqlite3_column_int(s.get(),0),sqlite3_column_int(s.get(),1),sqlite3_column_double(s.get(),2),
   reinterpret_cast<const char*>(sqlite3_column_text(s.get(),3)),reinterpret_cast<const char*>(sqlite3_column_text(s.get(),4)),
   sqlite3_column_int(s.get(),5)!=0});
 if(rc!=SQLITE_DONE) throw std::runtime_error(sqlite3_errmsg(db_.handle()));
 return out;
}

std::vector<std::string> LMSRepository::users() {
 Statement s(db_.handle(),"SELECT username || ' (' || role || ')' FROM Users ORDER BY username;");
 std::vector<std::string> out;
 int rc;
 while((rc=sqlite3_step(s.get()))==SQLITE_ROW)
  out.emplace_back(reinterpret_cast<const char*>(sqlite3_column_text(s.get(),0)));
 if(rc!=SQLITE_DONE) throw std::runtime_error(sqlite3_errmsg(db_.handle()));
 return out;
}