/* Standard C++ includes */
#include <stdlib.h>
#include <iostream>
#include <fstream>

/*
  Include directly the different
  headers from cppconn/ and mysql_driver.h + mysql_util.h
  (and mysql_connection.h). This will reduce your build time!
*/
#include "mysql_connection.h"

#include <cppconn/driver.h>
#include <cppconn/exception.h>
#include <cppconn/resultset.h>
#include <cppconn/statement.h>
#include <cppconn/prepared_statement.h>

using namespace std;

int main(void)
{
cout << endl;
cout << "Dumping table ..." << endl;

try {
  sql::Driver *driver;
  sql::Connection *con;
  //sql::Statement *stmt;
  sql::ResultSet *res;
  sql::PreparedStatement *pstmt;

  /* Create a connection */
  driver = get_driver_instance();
  con = driver->connect("tcp://127.0.0.1:3306", "root", "mysql1234");
  /* Connect to the MySQL test database */
  con->setSchema("g_22");

  
  /* Select in ascending order */
  pstmt = con->prepareStatement("SELECT * FROM alerts");
  res = pstmt->executeQuery();

  /* Fetch in reverse = descending order! */
  //res->afterLast();
  
  char insert_str[] = "INSERT INTO `alerts` (`actionid`, `eventid`, `userid`, `clock`, `mediatypeid`, `sendto`, `subject`, `message`, `status`, `retries`, `error`, `esc_step`, `alerttype`) VALUES";
  
  ofstream myfile;
  myfile.open ("alerts.sql");
  
  myfile << insert_str << endl;
  int sql_lines = 0;
  
  while (res->next()) {
    
    myfile << "(" << res->getInt("actionid") << ","
    << res->getInt("eventid") << ","
    << res->getInt("userid") << ","
    << "'" << res->getString("clock") << "',"
    << res->getInt("mediatypeid") << ","
    << "'" << res->getString("sendto") << "',"
    << "'" << res->getString("subject") << "',"
    << "'" << res->getString("message") << "',"
    << res->getInt("status") << ","
    << res->getInt("retries") << ","
    << "'" << res->getString("error") << "',"
    << res->getInt("esc_step") << ","
    << res->getInt("alerttype")
    << ")";
    
    sql_lines++;
    
    if (sql_lines > 99) {
    	myfile << ";" << endl << insert_str << endl;
    	sql_lines = 0;
    } else {
    	myfile << "," << endl;
    }
    
  }
  
  myfile.close();
  
  delete res;
  delete pstmt;
  delete con;

} catch (sql::SQLException &e) {
  cout << "# ERR: SQLException in " << __FILE__;
  cout << "(" << __FUNCTION__ << ") on line " << __LINE__ << endl;
  cout << "# ERR: " << e.what();
  cout << " (MySQL error code: " << e.getErrorCode();
  cout << ", SQLState: " << e.getSQLState() << " )" << endl;
}

cout << endl;

return EXIT_SUCCESS;
}
