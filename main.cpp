#include <iostream>
#include <cstring>
#include <string>
#include <fstream>
#include <filesystem>
#include <cstdio>
#include "sqlite3.h"
using namespace std;

void newDB(const char *);

int main(int argc, char *argv[])
{
    std::filesystem::create_directory("databases");
    if (strcmp(argv[1], "-c") == 0)
    {
        if(argc == 2)
        {
            string s;
            cout<<"Enter name of database: ";
            cin>>s;
            newDB(s.c_str());
        }
        newDB((const char *)argv[2]);
    }

    if (strcmp(argv[1], "-a") == 0)
    {
        if(argc == 2)
        {
            string s;
            
            cout<<"Index of Database to add to: ";
            cin>>s;
            
        }
    }
}

void newDB(const char *DBName)
{
    char path[100] = "databases\\";
    char ext[] = ".db";
    strcat(path, DBName);
    strcat(path, ext);
    ifstream file(path);
    if (file.good())
    {
        cerr << "Error: " << DBName << " already exists." << endl;
        file.close();
        return;
    }
    file.close();

    sqlite3 *db;
    char *err_msg = 0;
    int rc = sqlite3_open(path, &db);
    if (rc != SQLITE_OK)
    {
        cerr << "Error creating database: " << sqlite3_errmsg(db) << endl;
        sqlite3_close(db);
        return;
    }

    char pragma[] = "PRAGMA foreign_keys = ON;";
    rc = sqlite3_exec(db, pragma, 0, 0, &err_msg);
    if (rc != SQLITE_OK)
    {
        cerr << "Error enabling foreign keys: " << err_msg << endl;
        sqlite3_free(err_msg);
        sqlite3_close(db);
        return;
    }

    char sql1[] = "CREATE TABLE people(\n"
                    "   id INTEGER PRIMARY KEY NOT NULL,\n"
                    "   name VARCHAR(50) NOT NULL,\n"
                    "   gender CHAR(1)\n"
                    ");\n";
    rc = sqlite3_exec(db, sql1, 0, 0, &err_msg);
    if (rc != SQLITE_OK)
    {
        cerr << "SQL error creating 'people' table: " << err_msg << endl;
        sqlite3_free(err_msg);
        sqlite3_close(db);
        remove(path);
        return;
    }

    char sql2[] = "CREATE TABLE parent_child(\n"
                    "   father_id INTEGER,\n"
                    "   mother_id INTEGER,\n"
                    "   child_id INTEGER,\n"
                    "   FOREIGN KEY(father_id) REFERENCES people(id),\n"
                    "   FOREIGN KEY(mother_id) REFERENCES people(id),\n"
                    "   FOREIGN KEY(child_id) REFERENCES people(id)\n"
                    ");\n";
    rc = sqlite3_exec(db, sql2, 0, 0, &err_msg);
    if (rc != SQLITE_OK)
    {
        cerr << "SQL error creating parent_child table: " << err_msg << endl;
        sqlite3_free(err_msg);
        sqlite3_close(db);
        remove(path);
        return;
    }

    char sql3[] =   "CREATE TABLE spouse(\n"
                    "   husband_id INTEGER,\n"
                    "   wife_id INTEGER,\n"
                    "   FOREIGN KEY(husband_id) REFERENCES people(id),\n"
                    "   FOREIGN KEY(wife_id) REFERENCES people(id)\n"
                    ");\n";
    sqlite3_exec(db, sql3, 0, 0, &err_msg);
    if (rc != SQLITE_OK)
    {
        cerr << "SQL error creating spouse table: " << err_msg << endl;
        sqlite3_free(err_msg);
        sqlite3_close(db);
        remove(path);
        return;
    }
    cout<<"Created "<<DBName<<".db";
    sqlite3_close(db);
    return;
}