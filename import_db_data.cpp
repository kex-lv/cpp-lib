/* Standard C++ includes */
#include <stdlib.h>
#include <iostream>
#include <fstream>

#include <string.h>

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
cout << "Insert into trends uint ..." << endl;

try {
  sql::Driver *driver;
  sql::Connection *con;
  
  int trend_counter = 0;
  string str1, trend_records2[32][5];
  
  ifstream myfile;
  myfile.open ("../trends_uint_selected.sql");
  
  while (getline(myfile, str1, ',')) {
    trend_records2[trend_counter][0] = str1;
  	getline(myfile, str1, ',');
  	getline(myfile, trend_records2[trend_counter][1], ',');
  	getline(myfile, trend_records2[trend_counter][2], ',');
  	getline(myfile, trend_records2[trend_counter][3], ',');
  	getline(myfile, trend_records2[trend_counter][4]);
  	trend_counter++;
  }
  myfile.close();
  
  cout << "connecting ..." << endl;
  /* Create a connection */
  driver = get_driver_instance();
  con = driver->connect("tcp://sql:3306", "root", "");
  /* Connect to the MySQL test database */
  con->setSchema("g_trunk");

  cout << "run select ..." << endl;
  int min_clock = 1428830000;
  //1450708024
  
  for (int clock = 1440708024; clock > min_clock; clock--) {
      char select_query[100];
      sprintf(select_query, "SELECT NULL FROM trends_uint WHERE clock = %u", clock);
      
      sql::ResultSet *res;
      sql::PreparedStatement *pstmt;
      
	  pstmt = con->prepareStatement(select_query);
	  res = pstmt->executeQuery();
	  
	  if (0 == res->next()) {
	      char insert_str[10240], tmpstr[255];
	      sprintf(insert_str, "INSERT INTO `trends_uint` (`itemid`, `clock`, `num`, `value_min`, `value_avg`, `value_max`) VALUES ");
	      
	      for (int i = 0; i < 32; i++) {
	        sprintf(tmpstr, "\r\n(%s,%u,%s,%s,%s,%s),", trend_records2[i][0].c_str(), clock, trend_records2[i][1].c_str(), 
	        trend_records2[i][2].c_str(), trend_records2[i][3].c_str(), trend_records2[i][4].c_str());
	        
	        strcat(insert_str, tmpstr);
		  }
		  
		  insert_str[strlen(insert_str)-1] = ';';
		  
		  sql::PreparedStatement *insert;
		  sql::ResultSet *insert_res;
			  
		  insert = con->prepareStatement(insert_str);
		  insert_res = insert->executeQuery();
			  
		  delete insert;
		  delete insert_res;
	  	  
	  }
	  else
	  {
	      cout << clock << " yes" << endl;
	  }
	  
	  delete res;
      delete pstmt;
  }
  
  
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
