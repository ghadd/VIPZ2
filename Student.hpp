#pragma once 
#ifndef STUDENT_H
#define STUDENT_H

#include <iostream>
#include <string>
#include <algorithm>
#include <numeric>
#include "boost/lexical_cast.hpp"

#endif // !STUDENT_H


struct Credentials {
	std::string name;
	std::string surname;
};

struct Date
{
	typedef unsigned short int ushort;
	ushort day;
	ushort month;
	ushort year;

	// format dd/mm/yyyy
	void parse_from_std_string(std::string& string_to_parse) {
		try
		{
			this->day = boost::lexical_cast<short>(string_to_parse.substr(0, 2));
			this->month = boost::lexical_cast<short>(string_to_parse.substr(3, 2));
			this->year = boost::lexical_cast<short>(string_to_parse.substr(6, 4));
		}
		catch (const boost::bad_lexical_cast&)
		{
			std::cout << "Bad cast on date <" + string_to_parse << ">" << std::endl;
			this->day = 0;
			this->month = 0;
			this->year = 0;
		}
	}

	std::string toString() {
		return std::to_string(this->day / 10) + std::to_string(this->day % 10) + "/" +
			std::to_string(this->month / 10) + std::to_string(this->month % 10) + "/" + std::to_string(this->year);
	}
};

class Student {
public:
	Student() {};
	Student(Credentials&, Date&, std::vector<int>&);
	~Student() {};

	void set_credentials(std::string _name, std::string _surname) {
		this->creds.name = _name;
		this->creds.surname = _surname;
	}

	Credentials& get_credentials() {
		return this->creds;
	}

	void set_date(std::string _date_str) {
		this->date.parse_from_std_string(_date_str);
	}

	Date& get_date() {
		return this->date;
	}

	void set_grades(std::vector<int>& _grades) {
		this->grades = _grades;
	}

	std::vector<int>& get_grades() {
		return this->grades;
	}

private:
	Credentials creds;
	Date date;
	std::vector<int> grades;
};

Student::Student(Credentials& _creds, Date& _date, std::vector<int>& _grades) {
	this->creds = _creds;
	this->date = _date;
	this->grades = _grades;
}