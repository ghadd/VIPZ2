#pragma once

#ifndef STUDENT_DB_HPP
#define STUDENT_DB_HPP

#include <iostream>
#include <fstream>
#include "Student.hpp"

const std::string DB_PATH = "E:\\repos\\OOPLAB1\\OOPLAB1\\.students_db";

int getLines(std::string filename) {
	std::fstream file(filename, std::ios_base::in);
	if (!file.is_open()) {
		std::cout << "This file does not exist." << std::endl;
		exit(69);
	}
	int cnt = 0;
	std::string buffer;
	while (!file.eof()) {
		getline(file, buffer);
		cnt++;
	}
	file.close();
	return cnt;
}

std::string vecToString(std::vector<int> grades) {
    std::string res = "";
    for (auto& grade : grades) {
        res += std::to_string(grade) + " ";
    }
    return res;
}

bool addStudent(Student* st) {
    std::fstream file(DB_PATH, std::ios_base::app);
    if (!file.is_open()) return false;

    file << st->get_credentials().surname << " " << st->get_credentials().name << " " <<
        st->get_date().toString() << " " << vecToString(st->get_grades()) << std::endl;

    file.close();

    return true;
}

bool containsStudent(std::string name, std::string surname) {
	int lines = getLines(DB_PATH);
	std::fstream file(DB_PATH, std::ios_base::in);
	if (!file.is_open()) return false;
	std::string buffer;

	for (int i = 0; i < lines - 1; i++) {
		std::getline(file, buffer);
		size_t sp1 = buffer.find(' ');
		size_t sp2 = buffer.find(' ', sp1 + 1);
		std::string _surname = buffer.substr(0, sp1);
		std::string _name = buffer.substr(_surname.length() + 1, sp2 - sp1 - 1);

		if (_name == name && _surname == surname) {
			file.close();
			return true;
		}
	}
	file.close();
	return false;
}

std::string getStudent(std::string name, std::string surname) {
	if (!containsStudent(name, surname)) {
		return "No matching record.";
	}

	int lines = getLines(DB_PATH);
	std::fstream file(DB_PATH, std::ios_base::in);
	if (!file.is_open()) return "404";
	std::string buffer;

	for (int i = 0; i < lines - 1; i++) {
		std::getline(file, buffer);
		size_t sp1 = buffer.find(' ');
		size_t sp2 = buffer.find(' ', sp1 + 1);
		std::string _surname = buffer.substr(0, sp1);
		std::string _name = buffer.substr(_surname.length() + 1, sp2 - sp1 - 1);

		if (_name == name && _surname == surname) {
			return buffer;
		}
	}
	file.close();
	return "No matching record.";
}

bool deleteStudent(std::string name, std::string surname) {
	if (!containsStudent(name, surname)) return false;
	bool deleted = false;
	int lines = getLines(DB_PATH);
	std::fstream file(DB_PATH, std::ios_base::in);
	std::fstream temp("E:\\repos\\OOPLAB1\\OOPLAB1\\.tmp", std::ios_base::app);
	if (!file.is_open()) return "404";
	std::string buffer;

	for (int i = 0; i < lines - 1; i++) {
		std::getline(file, buffer);
		size_t sp1 = buffer.find(' ');
		size_t sp2 = buffer.find(' ', sp1 + 1);
		std::string _surname = buffer.substr(0, sp1);
		std::string _name = buffer.substr(_surname.length() + 1, sp2 - sp1 - 1);

		if (_name == name && _surname == surname) {
			deleted = true;
		}
		else {
			temp << buffer;
		}
	}
	temp.close();
	file.close();

	temp.open("E:\\repos\\OOPLAB1\\OOPLAB1\\.tmp", std::ios_base::in);
	if (!temp.is_open()) return false;
	file.open(DB_PATH, std::ios_base::out);
	if (!file.is_open()) return false;
	for (int i = 0; i < lines - 2; i++) {
		std::getline(temp, buffer);
		file << buffer << std::endl;
	}
	file.close();
	temp.close();
	remove("E:\\repos\\OOPLAB1\\OOPLAB1\\.tmp");
	return deleted;
}

#endif // !STUDENT_DB_HPP
