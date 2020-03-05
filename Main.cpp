#include "Student.hpp"
#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <list>
#include "Student_db.hpp"
#include "boost/lexical_cast.hpp"

const short NGRADES = 5;
bool running = true;
std::list<Student> students;

void closeDB(std::list<Student>&);

template<typename T>
double avg(std::vector<T> vec) {
	return (double)std::accumulate(vec.begin(), vec.end(), 0) / vec.size();
}

int countOf2(std::vector<int> vec) {
	return std::count(vec.begin(), vec.end(), 2);
}

void displayStudentInfo(Student* st) {
	std::cout << std::setw(10) << st->get_credentials().surname << " " << std::setw(10) << st->get_credentials().name << " " <<
		st->get_date().toString() << " " << vecToString(st->get_grades()) << std::endl;
}

void delete_losers(std::list<Student>& students) {
	int pos = 0;
	for (std::list<Student>::iterator it = students.begin(); it != students.end(); it = ++it) {
		if (countOf2((*it).get_grades()) == 2) {
			std::list<Student>::iterator to_delete = it++;
			students.erase(to_delete);
			it--;
		}
	}
}

void insert_sorted(std::list<Student>& students, std::string name, \
	std::string surname, std::string str_date, \
	std::vector<int> grades) {
	Student to_insert;
	to_insert.set_credentials(name, surname);
	to_insert.set_date(str_date);
	to_insert.set_grades(grades);

	if (std::find_if(grades.begin(), grades.end(), [](int x) -> bool {return x > 5 || x < 1; }) != grades.end()) {
		std::cout << "Skipping student with wrong grades set" << std::endl;
		for (int i = 0; i < 50; i++) std::cout << "="; std::cout << std::endl;
		displayStudentInfo(&to_insert);
		for (int i = 0; i < 50; i++) std::cout << "="; std::cout << std::endl;
		return;
	}

	if (students.empty()) {
		students.push_back(to_insert);
	}
	else if (avg(grades) > avg(students.front().get_grades())) {
		students.push_front(to_insert);
	}
	else {
		int pos;
		for (pos = 0; pos < students.size(); pos++) {
			auto it = std::next(students.begin(), pos);
			std::vector<int>& it_grades = (*it).get_grades();
			if (avg(it_grades) < avg(grades)) break;
		}
		students.insert(std::next(students.begin(), pos), to_insert);
	}
}

template <typename T, typename U>
void parseNumeralInput(const T& from, U& to) {
	try {
		to = boost::lexical_cast<U>(from);
	}
	catch (const boost::bad_lexical_cast&) {
		std::cout << "Invalid numeral. Commiting previous changes..." << std::endl;
		closeDB(students);
		exit(7);
	}
}

void readStudents(std::list<Student>& students, const std::string filename) {
	std::string strbuffer;
	char chrbuffer;
	std::string name, surname, str_date;
	if (filename != "kb") {
		int numberOfLines = getLines(filename);
		std::fstream file(filename, std::ios_base::in);
		for (int i = 0; i < numberOfLines; i++) {
			file >> surname >> name >> str_date;
			std::vector<int> buffer_grades(5);
			int grade;
			for (int j = 0; j < 5; j++) {
				file >> grade;
				buffer_grades[j] = grade;
			}
			insert_sorted(students, name, surname, str_date, buffer_grades);
		}
		file.close();
	}
	else {
		unsigned nStudents;
		std::cout << "Enter number of students to add: ";
		std::cin >> strbuffer;
		parseNumeralInput(strbuffer, nStudents);
		for (int i = 0; i < nStudents; i++) {
			std::cin >> surname >> name >> str_date;
			std::vector<int> buffer_grades(5);
			int grade;
			for (int j = 0; j < 5; j++) {
				std::cin >> chrbuffer;
				if (chrbuffer == ' ') {
					j--;
					continue;
				}
				parseNumeralInput(chrbuffer, grade);
				buffer_grades[j] = grade;
			}

			insert_sorted(students, name, surname, str_date, buffer_grades);
		}
	}
}

void deleteStudentFromListAndDB(std::list<Student>& students, std::string name, std::string surname) {
	std::list<Student>::iterator it = students.begin();
	while ((*it).get_credentials().name != name && (*it).get_credentials().surname != surname) {
		it++;
	}
	students.erase(it); // from the list
	deleteStudent(name, surname); // from the db
}

void loadDB(std::list<Student>& students) {
	readStudents(students, DB_PATH);
}

void closeDB(std::list<Student>& students) {
	std::fstream file(DB_PATH, std::ios_base::out);

	for (auto& st : students) {
		if (!containsStudent(st.get_credentials().name, st.get_credentials().surname)) {
			addStudent(&st);
		}
	}

	file.close();
}

void printMenu() {
	std::cout << "0: Display all students" << std::endl;
	std::cout << "1: Delete student from DB" << std::endl;
	std::cout << "2: Find student in DB" << std::endl;
	std::cout << "3: Delete losers from current session" << std::endl;
	std::cout << "4: Read students from file" << std::endl;
	std::cout << "5: Read students from keyboard" << std::endl;
	std::cout << "6: Commit all changes" << std::endl;
	std::cout << std::endl;
}

enum mode {
	_displayAll,
	_deleteStudent,
	_findStudent,
	_deleteLosers,
	_readFromFile,
	_readFromKb, 
	_commitChanges
};

int main() {
	loadDB(students);

	printMenu();
	while (running) {
		std::string strMode;
		std::string name, surname, filename;
		int mode;
		std::cin >> strMode;
		if (strMode == "!q") {
			running = false;
			break;
		}

		parseNumeralInput(strMode, mode);
		switch (mode) {
		case _displayAll:
			std::cout << "Displaying all students:" << std::endl;
			for (auto& student : students) {
				displayStudentInfo(&student);
			}
			std::cout << std::endl;
			break;
		case _deleteStudent:
			std::cout << "Enter name and surname to delete from database:" << std::endl;
			std::cin >> name >> surname;
			if (deleteStudent(name, surname)) {
				std::cout << "Deleting successful" << std::endl << std::endl;
			} 
			else {
				std::cout << "Deleting failure" << std::endl << std::endl;
			}
			break;
		case _findStudent:
			std::cout << "Enter name and surname to find in the database:" << std::endl;
			std::cin >> name >> surname;
			std::cout << getStudent(name, surname) << std::endl << std::endl;
			break;
		case _deleteLosers:
			std::cout << "Deleting stutents with 2 grades of 2" << std::endl;
			delete_losers(students);
			std::cout << "Successful" << std::endl << std::endl;
			break;
		case _readFromFile:
			std::cout << "Enter filename: ";
			std::cin >> filename;
			try {
				readStudents(students, filename);
				std::cout << "Successful" << std::endl << std::endl;
			}
			catch (...) {
				std::cout << "Unsuccessful" << std::endl << std::endl;
			}
			break;
		case _readFromKb:
			try {
				std::cout << "Enter students one by one" << std::endl;
				readStudents(students, "kb");
				std::cout << "Successful" << std::endl << std::endl;
			}
			catch (...) {
				std::cout << "Unsuccessful" << std::endl << std::endl;
			}
			break;
		case _commitChanges:
			closeDB(students);
			break;
		default:
			std::cout << "Unrecognized command" << std::endl;
			printMenu();
		}
	}

	closeDB(students);

	return 0;
}
