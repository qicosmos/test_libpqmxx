#include <postgres-connection.h>
#include <postgres-exceptions.h>

#include <iostream>

using namespace db::postgres;

int main() {

	Connection cnx;
	try {
		cnx.connect("hostaddr=192.168.3.226 dbname=exampledb user=dbuser password=12345 connect_timeout=5");

		cnx.execute(R"SQL(

			      DROP TABLE IF EXISTS employees;

			      CREATE TABLE employees (
        emp_no      INTEGER         NOT NULL,
        birth_date  DATE            NOT NULL,
        first_name  VARCHAR(14)     NOT NULL,
        last_name   VARCHAR(16)     NOT NULL,
        gender      "char"          NOT NULL,
        hire_date   DATE            NOT NULL,
        PRIMARY KEY (emp_no)
      );

			    )SQL");

		std::cout << "Table created." << std::endl;

		int64_t employees = cnx.execute(R"SQL(

			      INSERT INTO employees VALUES
        (10001,'1973-09-02','Georgi','Facello','M','2006-06-26'),
        (10002,'1984-06-02','Bezalel','Simmel','F','2005-11-21'),
        (10003,'1979-12-03','Parto','Bamford','M','2006-08-28'),
        (10004,'1974-05-01','Chirstian','Koblick','M','2006-12-01'),
        (10005,'1975-01-21','Kyoichi','Maliniak','M','2009-09-12'),
        (10006,'1973-04-20','Anneke','Preusig','F','2009-06-02'),
        (10007,'1977-05-23','Tzvetan','Zielinski','F','2009-02-10'),
        (10008,'1978-02-19','Saniya','Kalloufi','M','2014-09-15'),
        (10009,'1972-04-19','Sumant','Peac','F','2005-02-18'),
        (10010,'1983-06-01','Duangkaew','Piveteau','F','2009-08-24'),
        (10011,'1973-11-07','Mary','Sluis','F','2010-01-22'),
        (10012,'1980-10-04','Patricio','Bridgland','M','2012-12-18'),
        (10013,'1983-06-07','Eberhardt','Terkki','M','2005-10-20'),
        (10014,'1976-02-12','Berni','Genin','M','2007-03-11'),
        (10015,'1979-08-19','Guoxiang','Nooteboom','M','2007-07-02'),
        (10016,'1981-05-02','Kazuhito','Cappelletti','M','2015-01-27'),
        (10017,'1978-07-06','Cristinel','Bouloucos','F','2013-08-03'),
        (10018,'1984-06-19','Kazuhide','Peha','F','2007-04-03'),
        (10019,'1973-01-23','Lillian','Haddadi','M','2016-04-30'),
        (10020,'1972-12-24','Mayuko','Warwick','M','2011-01-26')

			    )SQL").count();

		std::cout << employees << " have been added." << std::endl;

		std::cout << "The three oldest employees are: " << std::endl;

		auto &oldest = cnx.execute(R"SQL(

			      SELECT first_name, last_name, DATE_PART('year', now()) - DATE_PART('year', birth_date)
        FROM employees
       ORDER BY birth_date
       LIMIT 3

			    )SQL");

		for (auto &row : oldest) {
			std::cout << "- " << row.get<std::string>(0) << " " << row.get<std::string>(1)<<", "<<row.get<std::string>("first_name")
				<< ", " << row.get<double>(2) << " years old." << std::endl;
		}

		auto &employee = cnx.execute(R"SQL(

			      SELECT first_name, last_name, DATE_PART('year', birth_date)
        FROM employees WHERE birth_date = $1::date

			    )SQL", "1973-11-07");

		std::cout << employee.get<std::string>(0) << " "
			<< employee.get<std::string>(1) << " is born in "
			<< employee.get<double>(2) << std::endl;

		int64_t deleted = cnx.execute(R"SQL(

			      DELETE FROM employees
        WHERE DATE_PART('year', birth_date) = $1 AND gender = $2

			    )SQL", 1973, 'M').count();

		std::cout << deleted << " employees records have been deleted." << std::endl;

		return 0;
	}
	catch (ConnectionException e) {
		std::cerr << "Oops... Cannot connect...";
	}
	catch (ExecutionException e) {
		std::cerr << "Oops... " << e.what();
	}

	return -1;
}