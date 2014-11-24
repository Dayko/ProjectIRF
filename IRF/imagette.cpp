#include "imagette.h"


void writeImagetteFile(string label, string scripter, string page, int row, int column, string imgFormat) {
	string fullPath = "../output/" + label + "_" + scripter + "_" + page + "_" + to_string(row) + "_" + to_string(column) + imgFormat ;

	// todo write img
}


void writeDescriptionFile(string label, string scripter, string page, int row, int column, IMAGETTE_SIZE size) {
	string fullPath = "../output/" + label + "_" + scripter + "_" + page + "_" + to_string(row) + "_" + to_string(column) + ".txt" ;

	ofstream file;
	file.open(fullPath);

	file << "# IRF Project - 2014 - G. Biez, A. Jaury, D. Le Guen, V. Pukari" << endl;
	file << "label " << label << endl;
	file << "form " << scripter << page << endl;
	file << "scripter " << scripter << endl;
	file << "page " << page << endl;
	file << "row " << row << endl;
	file << "column " << column << endl;
	file << "size " << to_string(size) << endl;

	file.close();
}