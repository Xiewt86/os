#include <bits/stdc++.h>
#include <unistd.h>
#include <string>
#include <list>
#include <vector>
#include <map>

using namespace std;

struct file *get_file (string fname);
struct user *get_user (string uname);
struct dir *get_dir (string dirname);
int operation (string op);
void mkdir (string dirname);
void ls ();
void lsp (string path);
void cd (string dirname);
int cdp (string path);
struct dir *find_dir (string path, struct dir *d);
int find_file (string fname, struct dir *d, int cnt);
void mkdirp (string path, string dirname);
void dedir (string dirname);
void subdedir (struct dir *target);
void dedirp (string path, string dirname);
void create (string fname, int omode);
void createp (string path, string fname, int omode);
void search_file (string fname);
void file_open (string fname);
void file_openp (string path, string fname);
void lsfile ();
void file_write (string fname, string buff, int wmode);
void file_writep (string path, string fname, string buff, int wmode);
void cat_file (string fname);
void cat_filep (string path, string fname);
bool file_can_write (struct file *f);
bool file_can_read (struct file *f);
void reposition (string fname, int pos);
void repositionp (string path, string fname, int pos);
void truncate (string fname, int cnt);
void truncatep (string path, string fname, int cnt);
void chmod (string fname, int omode);
void chmodp (string path, string fname, int omode);
void file_close (string fname);
void file_closep (string path, string fname);
void file_delete (string fname);
void file_deletep (string path, string name);
void regusr (string username, string password);
void delusr (string username);
void disusr ();
void login (string username);

struct dir
{
	string name;                     /* Directory name */
	string path;                     /* Path name */
	list <struct file*> files;       /* Files in dir */
	list <struct dir*> dirs;         /* Directories in dir */
};

struct file
{
	string name;         			 /* File name */
	string path;                     /* Path of the file */
	string content;      			 /* File content */
	int omode;           			 /* File operation mode */
	bool open;           			 /* File open or not */
	string creater;                  /* Creater of the file */
    string opener;					 /* Opener of the file */
	int ptr;                         /* File pointer */
};

struct user
{
	string username;				 /* User name */
	string password;				 /* User password */
};

struct dir *root_dir;                /* Root dir of the file system */
list <struct user*> user_list;       /* List of all users */
list <struct file*> fopen_list;      /* List of all opened file */
struct user *cur_usr;                /* Current user */
struct dir *cur_dir;                 /* Current directory */

int main ()
{   
	root_dir = new struct dir;       /* Initiate root directory */      
	root_dir->name = "";
	root_dir->path = "/";
	struct user *root = new struct user;  /* Superuser: root */
	root->username = "root";
	root->password = "111";
	cur_dir = root_dir;
	user_list.push_back(root);
	cur_usr = root;
	while (1)
	{
		string op;
		cout << cur_usr->username << "$" << cur_dir->path << "%:";
		cin >> op;
		if (!operation (op))
		{
			return 0;
		}
	}
}

/* Make a new directory */
void mkdir (string dirname)
{
	struct dir *target = get_dir (dirname);
	if (target != NULL)
	{
		cout << "directory already exists" << endl;
		return;
	}
	string path = cur_dir->path;
	path += dirname;
	path += "/";
	struct dir *subdir = new struct dir;
	subdir->name = dirname;
	subdir->path = path;
	cur_dir->dirs.push_back(subdir);
}

/* List files & dirs in the current dir */
void ls ()
{
	for (auto ditr = cur_dir->dirs.begin(); ditr != cur_dir->dirs.end(); ++ditr)
	{
		cout << (*ditr)->name << endl;
	}
	for (auto fitr = cur_dir->files.begin(); fitr != cur_dir->files.end(); ++fitr)
	{
		cout << (*fitr)->name << endl;
	}
}

/* List files & dirs in a given dir */
void lsp (string path)
{
	struct dir *cur_dir_tmp = cur_dir;
	if (cdp (path))
	{
		ls ();
		cur_dir = cur_dir_tmp;
	}
}

/* Change the current directory */
void cd (string dirname)
{
	struct dir *target = get_dir (dirname);
	if (target == NULL)
	{
		cout << "directory does not exist" << endl;
		return;
	}	
	cur_dir = target;
}

/* Change the current dir to the given dir */
int cdp (string path)
{
	struct dir *target_dir = find_dir (path, root_dir);
	if (target_dir == NULL)
	{
		cout << "directory does not exist" << endl;
		return 0;
	}
	cur_dir = target_dir;
	return 1;
}

/* Search file iteratively */
struct dir *find_dir (string path, struct dir *d)
{
	if (d->path == path)
	{
		return d;
	}
	struct dir *target = NULL;
	for (auto itr = d->dirs.begin(); itr != d->dirs.end(); ++itr)
	{
		target = find_dir (path, *itr);
		return target;
//		if (target != NULL)
//		{
//			return target;
//		}
	}
	return NULL;
}

/* Create a new dir at the given path */
void mkdirp (string path, string dirname)
{
	struct dir *cur_dir_tmp = cur_dir;
	if (cdp (path))
	{
		mkdir (dirname);
		cur_dir = cur_dir_tmp;
	}
}

/* Delete a dir */
void dedir (string dirname)
{
	struct dir *cur_dir_tmp = cur_dir;
	for (auto itr = cur_dir->dirs.begin(); itr != cur_dir->dirs.end(); ++itr)
	{
		if ((*itr)->name == dirname)
		{
			subdedir ((*itr));
			delete (*itr);
			cur_dir->dirs.erase(itr);
			cur_dir = cur_dir_tmp;
			return;
		}
	}
	cout << "directory does not exist" << endl;
}

/* Iteratively delete dirs & files when dedir() is called */
void subdedir (struct dir *target)
{
	cur_dir = target;
	for (auto fitr = target->files.begin(); fitr != target->files.end(); ++fitr)
	{
		if ((*fitr)->open)
		{
			for (auto ffitr = fopen_list.begin(); ffitr != fopen_list.end(); ++ffitr)
			{
				if ((*ffitr)->name == (*fitr)->name)
				{
					fopen_list.erase(ffitr);
					break;
				}
			}
		}
		delete *fitr;
	}
	for (auto ditr = target->dirs.begin(); ditr != target->dirs.end(); ++ditr)
	{
		subdedir (*ditr);
		cur_dir = target;
		delete (*ditr);
	}
	target->files.clear();
	target->dirs.clear();
}

/* Delete a dir at the given path */
void dedirp (string path, string dirname)
{
	struct dir *cur_dir_tmp = cur_dir;
	if (cdp (path))
	{
		dedir (dirname);
		cur_dir = cur_dir_tmp;
	}
}

/* Create a file */
void create (string fname, int omode)
{
	struct file *target = get_file (fname);
	if (target != NULL)
	{
		cout << "file already exists" << endl;
		return;
	}
	target = new struct file;
	target->name = fname;
	target->omode = omode;
	target->path = cur_dir->path;
	target->content = "";
	target->open = 0;
	target->creater = cur_usr->username;
	target->opener = "";
	target->ptr = 0;
	cur_dir->files.push_back (target);
}

/* Create a file at the given path */
void createp (string path, string fname, int omode)
{
	struct dir *cur_dir_tmp = cur_dir;
	if (cdp (path))
	{
		create (fname, omode);
		cur_dir = cur_dir_tmp;
	}
}

/* Search for files given the file name */
void search_file (string fname)
{	
	if (!find_file(fname, root_dir, 0))
	{
		cout << "file does not found" << endl;
	}
}

/* Iteratively search files when search_file() is called */
int find_file (string fname, struct dir *d, int cnt)
{
	int c = cnt;
	for (auto fitr = d->files.begin(); fitr != d->files.end(); ++fitr)
	{
		if ((*fitr)->name ==fname)
		{
			cout << (*fitr)->path + (*fitr)->name << endl;
			c++;
		}
	}
	for (auto ditr = d->dirs.begin(); ditr != d->dirs.end(); ++ditr)
	{
		find_file(fname, *ditr, c);
	}
	return c;
}

/* Open a file */
void file_open (string fname)
{
	struct file *target = get_file (fname);
	if (target == NULL)
	{
		cout << "file does not exist" << endl;
		return;
	}
	if (target->open)
	{
		cout << "file already open" << endl;
		return;
	}
	target->open = 1;
	target->opener = cur_usr->username;
	fopen_list.push_back (target);
}

/* Open a file at the given path */
void file_openp (string path, string fname)
{
	struct dir *cur_dir_tmp = cur_dir;
	if (cdp (path))
	{
		file_open (fname);
		cur_dir = cur_dir_tmp;
	}
}

/* List all the opened file */
void lsfile ()
{
	for (auto fitr = fopen_list.begin(); fitr != fopen_list.end(); ++fitr)
	{
		cout << (*fitr)->path << "\t" << (*fitr)->name << "\t" << (*fitr)->creater << "\t" << (*fitr)->opener << "\t" << (*fitr)->omode << endl;
	}
}

/* Write a file */
void file_write (string fname, string buff, int wmode)
{
	struct file *target = get_file (fname);
	if (target == NULL)
	{
		cout << "file does not exist" << endl;
		return;
	}
	if (!target->open)
	{
		cout << "file does not open" << endl;
		return;
	}
	if (!file_can_write (target))
	{
		cout << "permission denied" << endl;
		return;
	}
	/* 3 Writting modes */
	switch (wmode)
	{
		case 0:
			{
				target->content += buff;
				target->ptr = target->content.length();
				break;
			}
		case 1:
			{
				target->content.replace(target->ptr, buff.length(), buff);
				target->ptr = target->content.length();
				break;
			}
		case 2:
			{
				target->content.replace(0, buff.length(), buff);
				target->ptr = target->content.length();
				break;
			}
	}
	cout << "write ok" << endl;
}

/* Write a file at the given path */ 
void file_writep (string path, string fname, string buff, int wmode)
{
	struct dir *cur_dir_tmp = cur_dir;
	if (cdp (path))
	{
		file_write (fname, buff, wmode);
		cur_dir = cur_dir_tmp;
	}
}

/* Reposition the file pointer */
void reposition (string fname, int pos)
{
	struct file *target = get_file (fname);
	if (target == NULL)
	{
		cout << "file does not exist" << endl;
		return;
	}
	if (pos < 0 || pos > target->content.length())
	{
		cout << "reposition failed" << endl;
		return;
	}
	target->ptr = pos;
	cout << "reposition ok" << endl;
}

/* Reposition the file (at a given path) pointer */
void repositionp (string path, string fname, int pos)
{
	struct dir *cur_dir_tmp = cur_dir;
	if (cdp (path))
	{
		reposition (fname, pos);
		cur_dir = cur_dir_tmp;
	}
}

/* Print the file content */
void cat_file (string fname)
{
	struct file *target = get_file (fname);
	if (target == NULL)
	{
		cout << "file does not exist" << endl;
		return;
	}
	if (!target->open)
	{
		cout << "file does not open" << endl;
		return;
	}
	if (!file_can_read (target))
	{
		cout << "permission denied" << endl;
		return;
	}
	cout << target->content << endl;
}

/* Print the file (at a given path) content */
void cat_filep (string path, string fname)
{
	struct dir *cur_dir_tmp = cur_dir;
	if (cdp (path))
	{
		cat_file (fname);
		cur_dir = cur_dir_tmp;
	}
}

/* Truncate the file content */
void truncate (string fname, int cnt)
{
	struct file *target = get_file (fname);
	if (target == NULL)
	{
		cout << "file does not exist" << endl;
		return;
	}
	target->content = target->content.substr (0, cnt);
}

/* Truncate the file (at a given path) content */
void truncatep (string path, string fname, int cnt)
{
	struct dir *cur_dir_tmp = cur_dir;
	if (cdp (path))
	{
		truncate (fname, cnt);
		cur_dir = cur_dir_tmp;
	}
}

/* Change the operation mode of a file */
void chmod (string fname, int omode)
{
	struct file *target = get_file (fname);
	if (target == NULL)
	{
		cout << "file does not exist" << endl;
	}
	target->omode = omode;
}

/* Change the operation mode if a file (at a given path) */
void chmodp (string path, string fname, int omode)
{
	struct dir *cur_dir_tmp = cur_dir;
	if (cdp (path))
	{
		chmod (fname, omode);
		cur_dir = cur_dir_tmp;
	}
}

/* Close a file */
void file_close (string fname)
{
	struct file *target = get_file (fname);
	if (target == NULL)
	{
		cout << "file does not exist" << endl;
		return;
	}
	if (target->open)
	{
		target->open = 0;
		for (auto fitr = fopen_list.begin(); fitr != fopen_list.end(); ++fitr)
		{
			if ((*fitr)->path+(*fitr)->name == cur_dir->path+fname)
			{
				cur_dir->files.erase(fitr);
				break;
			}
		}
	}
}

/* Close a file (at a given path) */
void file_closep (string path, string fname)
{
	struct dir *cur_dir_tmp = cur_dir;
	if (cdp (path))
	{
		file_close (fname);
		cur_dir = cur_dir_tmp;
	}
}

/* Delete a file */
void file_delete (string fname)
{
	for (auto fitr = cur_dir->files.begin(); fitr != cur_dir->files.end(); ++fitr)
	{
		if ((*fitr)->name == fname)
		{
			struct file *target = (*fitr);
			if (target->open)
			{
				file_close (fname);
			}
			cur_dir->files.erase (fitr);
			delete target;
			return;
		}
	}
	cout << "file does not exist" << endl;
}

/* Delete a file (at a given path) */
void file_deletep (string path, string fname)
{
	struct dir *cur_dir_tmp = cur_dir;
	if (cdp (path))
	{
		file_delete (fname);
		cur_dir = cur_dir_tmp;
	}
}

/* User registration */
void regusr (string username, string password)
{
	if (get_user (username) != NULL)
	{
		cout << "user already exists" << endl;
		return;
	}
	struct user *u = new struct user;
	u->username = username;
	u->password = password;
	user_list.push_back (u);
	cout << "register ok" << endl;
}

/* Delete a user */
void delusr (string username)
{
	struct user *target = get_user (username);
	if (target == NULL)
	{
		cout << "user does not exist" << endl;
		return;
	}
	if (cur_usr->username != "root")
	{
		cout << "permission denied" << endl;
		return;
	}
	for (auto uitr = user_list.begin(); uitr != user_list.end(); ++uitr)
	{
		if ((*uitr)->username == username)
		{
			user_list.erase (uitr);
			break;
		}
	}
	delete target;
}

/* Display the users info */
void disusr ()
{
	if (cur_usr->username != "root")
	{
		cout << "permission denied" << endl;
		return;
	}
	for (auto uitr = user_list.begin(); uitr != user_list.end(); ++uitr)
	{
		cout << (*uitr)->username << endl;
	}
}

/* Log in */
void login (string username)
{
	struct user *target = get_user (username);
	if (target == NULL)
	{
		cout << "user does not exist" << endl;
		return;
	}
	string password;
	cout << "password:";
	cin >> password;
	if (target->password == password)
	{
		cur_usr = target;
		cout << "login ok" << endl;
	}
	else 
	{
		cout << "password wrong" << endl;
	}
}

/* Return the target user pointer */
struct dir *get_dir (string dname)
{
	for (auto ditr = cur_dir->dirs.begin(); ditr != cur_dir->dirs.end(); ++ditr)
	{
		if ((*ditr)->name == dname)
		{
			return *ditr;
		}
	}
	return NULL;
}

/* Return the target file pointer */
struct file *get_file (string fname)
{
	for (auto fitr = cur_dir->files.begin(); fitr != cur_dir->files.end(); ++fitr)
	{
		if ((*fitr)->name == fname)
		{
			return *fitr;
		}
	}
	return NULL;
}

/* Return the target user pointer */
struct user *get_user (string uname)
{
	for (auto uitr = user_list.begin(); uitr != user_list.end(); ++uitr)
	{
		if ((*uitr)->username == uname)
		{
			return *uitr;
		}
	}
	return NULL;
}

/* Judging whether the file is writable */
bool file_can_write (struct file *f)
{
	switch (f->omode)
	{
		case 0: { return false; }
		case 1: { return false; }
		case 2: { return true;  }
		case 3: { return true;  }
		case 4: { return false; }
		case 5: { return false; }
		case 6: { return true;  }
		case 7: { return true;  }
	}
}

/* Judging whether the file is readable */
bool file_can_read (struct file *f)
{
	switch (f->omode)
	{
		case 0: { return false;	}
		case 1: { return false; }
		case 2: { return false;	}
		case 3: { return false;	}
		case 4: { return true;  }
		case 5: { return true;  }
		case 6: { return true;  }		
		case 7: { return true;	}
	}
}

/* Transfer the command to function */
int operation (string op)
{
	if (op == "man")
	{
		cout << "Manual:" << endl;
		cout << "0) pwd (current work directory)" << endl;
		cout << "1) mkdir(p) (path) directory_name" << endl;
		cout << "2) dedir(p) (path) directory_name" << endl;
		cout << "3) search file_name" << endl;
		cout << "4) create(p) (path) file_name omode" << endl;
		cout << "5) cat(p) (path) file_name" << endl;
		cout << "6) delete(p) (path) file_name" << endl;
		cout << "7) write(p) (path) file_name buff wmode" << endl;
		cout << "8) reposition(p) (path) file_name pos" << endl;
		cout << "9) truncate(p) (path) file_name cnt" << endl;
		cout << "10) ls(p) (path)" << endl;
		cout << "11) open(p) (path) file_name" << endl;
		cout << "12) close(p) (path) file_name" << endl;
		cout << "13) regusr user_name password" << endl;
		cout << "14) delusr user_name [root only]" << endl;
		cout << "15) disusr" << endl;
		cout << "16) login user_name" << endl;
		cout << "17) lsfile" << endl;
		cout << "18) chmod(p) (path) file_name mode" << endl;
		cout << "19) cd(p) (path) dirname" << endl;
		cout << "20) exit" << endl;
	}
	else if (op == "exit")
	{
		return 0;
	}
	else if (op == "pwd")
	{
		cout << cur_dir->path << endl;
	}
	else if (op == "mkdir")
	{
		string dirname;
		cin >> dirname;
		mkdir (dirname);
	}
	else if (op == "ls")
	{
		ls ();
	}
	else if (op == "lsp")
	{
		string path;
		cin >> path;
		lsp (path);
	}
	else if (op == "cd")
	{
		string dirname;
		cin >> dirname;
		cd (dirname);
	}
	else if (op == "cdp")
	{
		string path;
		cin >> path;
		cdp (path);
	}
	else if (op == "mkdirp")
	{
		string path;
		string dirname;
		cin >> path >> dirname;
		mkdirp (path, dirname);
	}
	else if (op == "dedir")
	{
		string dirname;
		cin >> dirname;
		dedir (dirname);
	}
	else if (op == "dedirp")
	{
		string path;
		string dirname;
		cin >> path >> dirname;
		dedirp (path, dirname);
	}
	else if (op == "create")
	{
		string fname;
		string omode;
		cin >> fname >> omode;
		create (fname, atoi(omode.c_str()));
	}
	else if (op == "createp")
	{
		string path;
		string fname;
		string omode;
		cin >> path >> fname >> omode;
		createp (path, fname, atoi(omode.c_str()));
	}
	else if (op == "search")
	{
		string fname;
		cin >> fname;
		search_file (fname);
	}
	else if (op == "open")
	{
		string fname;
		cin >> fname;
		file_open (fname);
	}
	else if (op == "openp")
	{
		string path;
		string fname;
		cin >> path >> fname;
		file_openp (path, fname);
	}
	else if (op == "lsfile")
	{
		lsfile ();
	}
	else if (op == "write")
	{
		string fname;
		string buff;
		string wmode;
		cin >> fname >> buff >> wmode;
		file_write (fname, buff, atoi(wmode.c_str()));
	}
	else if (op == "writep")
	{
		string path;
		string fname;
		string buff;
		string wmode;
		cin >> path >> fname >> buff >> wmode;
		file_writep (path, fname, buff, atoi(wmode.c_str()));
	}
	else if (op == "reposition")
	{
		string fname;
		string pos;
		cin >> fname >> pos;
		reposition (fname, atoi(pos.c_str()));
	}
	else if (op == "repositionp")
	{
		string path;
		string fname;
		string pos;
		cin >> path >> fname >> pos;
		repositionp (path, fname, atoi(pos.c_str()));
	}
	else if (op == "cat")
	{
		string fname;
		cin >> fname;
		cat_file (fname);
	}
	else if (op == "catp")
	{
		string path;
		string fname;
		cin >> path >> fname;
		cat_filep (path, fname);
	}
	else if (op == "truncate")
	{
		string fname;
		string cnt;
		cin >> fname >> cnt;
		truncate (fname, atoi(cnt.c_str()));
	}
	else if (op == "truncatep")
	{
		string path;
		string fname;
		string cnt;
		cin >> path >> fname >> cnt;
		truncatep (path, fname, atoi(cnt.c_str()));
	}
	else if (op == "chmod")
	{
		string fname;
		string omode;
		cin >> fname >> omode;
		chmod (fname, atoi(omode.c_str()));
	}
	else if (op == "chmodp")
	{
		string path;
		string fname;
		string omode;
		cin >> path >> fname >> omode;
		chmodp (path, fname, atoi(omode.c_str()));
	}
	else if (op == "close")
	{
		string fname;
		cin >> fname;
		file_close (fname);
	}
	else if (op == "closep")
	{
		string path;
		string fname;
		cin >> path >> fname;
		file_closep (path, fname);
	}
	else if (op == "delete")
	{
		string fname;
		cin >> fname;
		file_delete (fname);
	}
	else if (op == "deletep")
	{
		string path;
		string fname;
		cin >> path >> fname;
		file_deletep (path, fname);
	}
	else if (op == "regusr")
	{
		string username;
		string password;
		cin >> username >> password;
		regusr (username, password);
	}
	else if (op == "delusr")
	{
		string username;
		cin >> username;
		delusr (username);
	}
	else if (op == "disusr")
	{
		disusr ();
	}
	else if (op == "login")
	{
		string username;
		cin >> username;
		login (username);
	}
	return 1;
}


