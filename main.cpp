#include <list>
#include <ctime>
#include <vector>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <unordered_map>
#include <fstream>
#include <algorithm>

using namespace std;

string curr_time();

class TreeNode
{
public:
    string name;
    list<string> contents;
    char type;
    string cdate;
    string mdate;
    int permission;
    TreeNode *parent;
    TreeNode *link;
    TreeNode *child;

    TreeNode(TreeNode *pwd, const std::string &name)
        : name(name), parent(pwd), permission(6),
          cdate(curr_time()), mdate(curr_time()),
          link(nullptr), child(nullptr) {}

    std::string get_permission() const
    {
        static const std::unordered_map<int, std::string> permissions = {
            {0, "---"}, {1, "--x"}, {2, "-w-"}, {3, "-wx"}, {4, "r--"}, {5, "r-x"}, {6, "rw-"}, {7, "rwx"}};

        auto it = permissions.find(permission);
        return (it != permissions.end()) ? it->second : "---";
    }

    ~TreeNode() = default;
};
void saveDFS(TreeNode* node, string path, ofstream &file);
TreeNode* loadFileSystem();
void saveFileSystem(TreeNode* root);
void linux_tree(TreeNode *root);
void print_help();
void print_tree(TreeNode *root, string prev);
void print_ls(TreeNode *pwd);
void print_stat(TreeNode *root, TreeNode *pwd, string path);
string pwd_str(TreeNode *root, TreeNode *pwd);
list<string> find_names(TreeNode *root, TreeNode *pwd, string name);
TreeNode *find_node(TreeNode *root, TreeNode *pwd, string path);
TreeNode *find_on_pwd(TreeNode *pwd, string name);
list<string> split(string str, char delim);
string join(list<string> str, char delim);
string *split_name(string str);
TreeNode *cd(TreeNode *root, TreeNode *pwd, string path);
TreeNode *create(TreeNode *root, TreeNode *pwd, string path, char type);
void remove(TreeNode *root, TreeNode *pwd, string path);
void dupl(TreeNode *root, TreeNode *pwd, string src, string dst, int keep);
void edit(TreeNode *root, TreeNode *pwd, string path);
void cat(TreeNode *root, TreeNode *pwd, string path);
void chmod(TreeNode *root, TreeNode *pwd, string path, string new_modes);
void clear_screen();
int compileFile(TreeNode *root, TreeNode *pwd, string path);
void runFile(TreeNode *root, TreeNode *pwd, string path);
void smartFind(TreeNode* root, string query);
void collectFiles(TreeNode* node, string path,vector<pair<string,string>>& files);
string buildJSON(vector<pair<string,string>>& files, string query);
string sendRequest(string jsonData);

int main()
{
    TreeNode *root = loadFileSystem();

    if (root == nullptr) {
        root = new TreeNode(nullptr, "");
        root->type = 'd';
        linux_tree(root);   // only first time
        saveFileSystem(root);
    }

    TreeNode *pwd = root;

    cout << endl;
    print_help();

    std::string cmd;
    std::cout << std::endl
              << pwd_str(root, pwd) << ">> ";
    while (std::getline(std::cin >> std::ws, cmd)) //ws: leading white spaces
    {
        std::list<std::string> args = split(cmd, ' ');
        TreeNode *temp_pwd = nullptr;

        if (cmd == "help")
        {
            print_help();
        }
        else if (args.front() == "ls")
        {
            args.pop_front();
            if (args.empty())
            {
                print_ls(pwd->child);
            }
            else
            {
                for (const std::string &arg : args)
                {
                    std::cout << arg << ":" << std::endl;
                    temp_pwd = cd(root, pwd, arg);
                    if (temp_pwd != nullptr)
                    {
                        print_ls(temp_pwd->child);
                    }
                }
            }
        }
        else if (args.front() == "tree")
        {
            args.pop_front();
            if (args.empty())
            {
                print_tree(pwd->child, "");
            }
            else
            {
                for (const std::string &arg : args)
                {
                    std::cout << arg << ":\n.\n";
                    temp_pwd = cd(root, pwd, arg);
                    if (temp_pwd != nullptr)
                    {
                        print_tree(temp_pwd->child, "");
                    }
                }
            }
        }
        else if (cmd == "pwd")
        {
            std::cout << pwd_str(root, pwd) << std::endl;
        }
        else if (args.front() == "cd")
        {
            args.pop_front();
            if (args.empty())
            {
                pwd = root;
            }
            else
            {
                temp_pwd = cd(root, pwd, args.front());
                if (temp_pwd != nullptr)
                {
                    pwd = temp_pwd;
                }
            }
        }
        else if (args.front() == "find")
        {
            args.pop_front();
            if (args.empty())
            {
                std::cout << "find: missing operand" << std::endl;
            }
            else
            {
                for (const std::string &arg : args)
                {
                    std::list<std::string> res;
                    if (arg[0] == '/')
                    {
                        res = find_names(root, root, arg);
                    }
                    else
                    {
                        res = find_names(pwd, pwd, arg);
                    }
                    if (res.empty())
                    {
                        std::cout << "find: '" << arg << "': no such file or directory" << std::endl;
                    }
                    else
                    {
                        for (const std::string &path : res)
                        {
                            std::cout << path << std::endl;
                        }
                    }
                }
            }
        }
        else if (args.front() == "stat")
        {
            args.pop_front();
            if (args.empty())
            {
                std::cout << "stat: missing operand" << std::endl;
            }
            else
            {
                for (const std::string &arg : args)
                {
                    print_stat(root, pwd, arg);
                }
            }
        }
        else if (args.front() == "mkdir")
        {
            args.pop_front();
            if (args.empty())
            {
                std::cout << "mkdir: missing operand" << std::endl;
            }
            else
            {
                for (const std::string &arg : args)
                {
                    create(root, pwd, arg, 'd');
                }
            }
        }
        else if (args.front() == "touch")
        {
            args.pop_front();
            if (args.empty())
            {
                std::cout << "touch: missing operand" << std::endl;
            }
            else
            {
                for (const std::string &arg : args)
                {
                    create(root, pwd, arg, '-');
                }
            }
        }
        else if (args.front() == "rm" || args.front() == "rmdir")
        {
            std::string command = args.front();
            args.pop_front();
            if (args.empty())
            {
                std::cout << command << ": missing operand" << std::endl;
            }
            else
            {
                for (const std::string &arg : args)
                {
                    remove(root, pwd, arg);
                }
            }
        }
        else if (args.front() == "cp" || args.front() == "mv")
        {
            std::string command = args.front();
            args.pop_front();
            if (args.size() != 2)
            {
                std::cout << command << ": missing operand" << std::endl;
            }
            else
            {
                int keep = (command == "cp") ? 1 : 0;
                std::string src = args.front();
                args.pop_front();
                std::string dst = args.front();
                args.pop_front();
                dupl(root, pwd, src, dst, keep);
            }
        }
        else if (args.front() == "edit")
        {
            args.pop_front();
            if (args.empty())
            {
                std::cout << "edit: missing operand" << std::endl;
            }
            else
            {
                edit(root, pwd, args.front());
            }
        }
        else if (args.front() == "cat")
        {
            args.pop_front();
            if (args.empty())
            {
                std::cout << "cat: missing operand" << std::endl;
            }
            else
            {
                cat(root, pwd, args.front());
            }
        }
        else if (args.front() == "chmod")
        {
            args.pop_front();
            if (args.size() < 2)
            {
                std::cout << "chmod: missing operand" << std::endl;
            }
            else
            {
                std::string perm = args.front();
                args.pop_front();
                chmod(root, pwd, args.front(), perm);
            }
        }
        else if (cmd == "clear")
        {
            clear_screen();
        }
        else if (cmd == "exit")
        {
            break;
        }
        else if (args.front() == "compile")
        {
            args.pop_front();
            if (args.empty())
            {
                cout << "compile: missing operand\n";
            }
            else
            {
                compileFile(root, pwd, args.front());
            }
        }
        else if (args.front() == "run")
        {
            args.pop_front();
            if (args.empty())
            {
                cout << "run: missing operand\n";
            }
            else
            {
                runFile(root, pwd, args.front());
            }
        }
        else if (args.front() == "smartfind")
        {
            args.pop_front();

            if (args.empty())
            {
                cout << "smartfind: missing query\n";
            }
            else
            {
                string query = "";
                for (auto &w : args)
                    query += w + " ";

                smartFind(root, query);
            }
        }
        else
        {
            std::cout << "Unknown command" << std::endl;
        }

        saveFileSystem(root);
        std::cout << std::endl
                  << pwd_str(root, pwd) << ">> ";
    }

    delete root;

    std::cout << std::endl;
    return 0;
}

void print_help()
{
    std::cout << "*** Follows the syntax of Linux shell commands ***" << std::endl
              << std::endl;
    std::cout << "\thelp      -   print this message" << std::endl;
    std::cout << "\tls        -   list contents of the current directory" << std::endl;
    std::cout << "\ttree      -   list contents of the current directory in a tree-like format" << std::endl;
    std::cout << "\tpwd       -   print the current working directory" << std::endl;
    std::cout << "\tcd DIR    -   change directory to DIR" << std::endl;
    std::cout << "\tfind N    -   find file or directory named N" << std::endl;
    std::cout << "\tstat P    -   print metadata of file or directory at path P" << std::endl;
    std::cout << "\tmkdir D   -   create a directory named D" << std::endl;
    std::cout << "\ttouch F   -   create a file named F" << std::endl;
    std::cout << "\trm P      -   remove the file or directory at path P" << std::endl;
    std::cout << "\trmdir P   -   remove the directory at path P" << std::endl;
    std::cout << "\tcp S D    -   copy file or directory from S to D" << std::endl;
    std::cout << "\tmv S D    -   move file or directory from S to D" << std::endl;
    std::cout << "\tedit P    -   edit the file at path P" << std::endl;
    std::cout << "\tcat P     -   print the contents of the file at path P" << std::endl;
    std::cout << "\tchmod M P -   change permissions of the file at path P to mode M" << std::endl;
    std::cout << "\tclear     -   clear the console screen" << std::endl;
    std::cout << "\texit      -   exit the shell" << std::endl;
}

void saveDFS(TreeNode* node, string path, ofstream &file) {
    if (!node) return;

    string fullPath = path + "/" + node->name;

    // convert contents to single string
    string contentStr = "";
    for (auto &line : node->contents) {
        contentStr += line + "\\n";
    }

    file << fullPath << "|"
         << node->type << "|"
         << node->permission << "|"
         << node->cdate << "|"
         << node->mdate << "|"
         << contentStr << "\n";

    saveDFS(node->child, fullPath, file);
    saveDFS(node->link, path, file);
}

void saveFileSystem(TreeNode* root) {
    ofstream file("filesystem.txt.tmp");

    saveDFS(root, "", file);

    file.close();

    remove("filesystem.txt");
    rename("filesystem.txt.tmp", "filesystem.txt");
}



TreeNode* loadFileSystem() {
    ifstream file("filesystem.txt");

    if (!file) return nullptr;

    TreeNode* root = new TreeNode(nullptr, "");
    root->type = 'd';

    string line;
    while (getline(file, line)) {
        stringstream ss(line);

        string path, type, perm, cdate, mdate, content;

        getline(ss, path, '|');
        getline(ss, type, '|');
        getline(ss, perm, '|');
        getline(ss, cdate, '|');
        getline(ss, mdate, '|');
        getline(ss, content);

        TreeNode* node = create(root, root, path.substr(1), type[0]);

        if (node) {
            node->permission = stoi(perm);
            node->cdate = cdate;
            node->mdate = mdate;

            // restore content
            stringstream cs(content);
            string line;
            while (getline(cs, line)) {
                node->contents.push_back(line);
            }
        }
    }

    return root;
}

void linux_tree(TreeNode *root)
{
    // Create some directories and files for demonstration purposes
    TreeNode *home = create(root, root, "home", 'd');
    TreeNode *user = create(root, home, "user", 'd');
    TreeNode *docs = create(root, user, "documents", 'd');
    create(root, docs, "file1.txt", '-');
    create(root, docs, "file2.txt", '-');
    TreeNode *pics = create(root, user, "pictures", 'd');
    create(root, pics, "image1.jpg", '-');
    create(root, pics, "image2.png", '-');
}

void print_tree(TreeNode *pwd, string prev)
{
    if (pwd == nullptr)
    {
        return;
    }
    else if (pwd->name.empty())
    {
        print_tree(pwd->child, prev);
        return;
    }
    string name = prev + "/" + pwd->name;
    cout << name << endl;
    print_tree(pwd->child, name);
    print_tree(pwd->link, prev);
}

void print_ls(TreeNode *pwd)
{
    if (pwd == nullptr)
    {
        return;
    }
    else if (pwd->name.empty())
    {
        print_ls(pwd->child);
        return;
    }
    cout << pwd->name << "\t" << pwd->type << pwd->get_permission() << "\t" << pwd->mdate << endl;
    print_ls(pwd->link);
}

void print_stat(TreeNode *root, TreeNode *pwd, string path)
{
    TreeNode *temp = find_node(root, pwd, path);
    if (temp != nullptr)
    {
        cout << "File: " << temp->name << endl;
        cout << "Type: " << temp->type << endl;
        cout << "Permission: " << temp->get_permission() << endl;
        cout << "Created: " << temp->cdate << endl;
        cout << "Modified: " << temp->mdate << endl;
    }
    else
    {
        cout << "stat: cannot stat '" << path << "': No such file or directory" << endl;
    }
}

string pwd_str(TreeNode *root, TreeNode *pwd)
{
    string path = pwd->name;
    while (pwd->parent != nullptr)
    {
        pwd = pwd->parent;
        path = pwd->name + "/" + path;
    }
    if (path.empty())
    {
        return "/";
    }
    return path;
}

list<string> find_names(TreeNode *root, TreeNode *pwd, string name)
{
    list<string> res;
    if (pwd == nullptr)
    {
        return res;
    }
    if (pwd->name == name)
    {
        res.push_back(pwd_str(root, pwd));
    }
    list<string> res1 = find_names(root, pwd->child, name);
    list<string> res2 = find_names(root, pwd->link, name);
    res.insert(res.end(), res1.begin(), res1.end());
    res.insert(res.end(), res2.begin(), res2.end());
    return res;
}

TreeNode *find_node(TreeNode *root, TreeNode *pwd, string path)
{
    // 🔥 DIRECT FILE IN CURRENT DIRECTORY
    if (path.find('/') == string::npos)
    {
        return find_on_pwd(pwd->child, path);
    }

    list<string> paths = split(path, '/');
    string last = paths.back();
    paths.pop_back();
    string prev = join(paths, '/');

    TreeNode *temp = cd(root, pwd, prev);
    if (!temp) return nullptr;

    return find_on_pwd(temp->child, last);
}

TreeNode *find_on_pwd(TreeNode *pwd, string name)
{
    if (pwd == nullptr)
    {
        return nullptr;
    }
    if (pwd->name == name)
    {
        return pwd;
    }
    return find_on_pwd(pwd->link, name);
}

list<string> split(string str, char delim)
{
    list<string> res;
    string temp;
    for (char ch : str)
    {
        if (ch == delim)
        {
            if (!temp.empty())
            {
                res.push_back(temp);
                temp.clear();
            }
        }
        else
        {
            temp.push_back(ch);
        }
    }
    if (!temp.empty())
    {
        res.push_back(temp);
    }
    return res;
}

string join(list<string> str, char delim)
{
    string res;
    for (const string &s : str)
    {
        if (!res.empty())
        {
            res.push_back(delim);
        }
        res += s;
    }
    return res;
}

string *split_name(string str)
{
// Splits the string into two parts at the last '/' — [directory, filename].
// Returns a new string array of size 2; caller must delete[] the returned pointer.

    size_t pos = str.find_last_of('/');
    if (pos == string::npos)
    {
        return new string[2]{"", str};
    }
    return new string[2]{str.substr(0, pos), str.substr(pos + 1)};
}

TreeNode *cd(TreeNode *root, TreeNode *pwd, string path)
{
    if (path.empty())
    {
        return pwd;
    }
    if (path[0] == '/')
    {
        pwd = root;
        path = path.substr(1);
    }
    list<string> paths = split(path, '/');
    for (const string &dir : paths)
    {
        if (dir == ".")
        {
            continue;
        }
        if (dir == "..")
        {
            if (pwd->parent != nullptr)
            {
                pwd = pwd->parent;
            }
            continue;
        }
        pwd = find_on_pwd(pwd->child, dir);
        if (pwd == nullptr)
        {
            std::cout << "cd: " << path << ": No such file or directory" << std::endl;
            return nullptr;
        }
    }
    return pwd;
}

TreeNode *create(TreeNode *root, TreeNode *pwd, string path, char type)
{
    // 🔹 split path first
    string *paths = split_name(path);

    // 🔹 find directory
    TreeNode *dir = cd(root, pwd, paths[0]);
    if (dir == nullptr)
    {
        return nullptr;
    }

    // 🔥 CHECK IF FILE ALREADY EXISTS (NOW SAFE)
    if (find_on_pwd(dir->child, paths[1]) != nullptr)
    {
        cout << "create: '" << path << "' already exists\n";
        return nullptr;
    }

    // 🔹 create new node
    TreeNode *newNode = new TreeNode(dir, paths[1]);
    newNode->type = type;
    newNode->link = dir->child;
    dir->child = newNode;

    if (type == 'd')
    {
        cout << "mkdir: created directory '" << path << "'" << endl;
    }
    else
    {
        cout << "touch: created file '" << path << "'" << endl;
    }

    return newNode;
}

void remove(TreeNode *root, TreeNode *pwd, string path)
{
    string *paths = split_name(path);
    TreeNode *dir = cd(root, pwd, paths[0]);
    if (dir == nullptr)
    {
        return;
    }
    TreeNode *prev = nullptr;
    TreeNode *curr = dir->child;
    while (curr != nullptr && curr->name != paths[1])
    {
        prev = curr;
        curr = curr->link;
    }
    if (curr == nullptr)
    {
        std::cout << "rm: " << path << ": No such file or directory" << std::endl;
        return;
    }
    if (curr->type == 'd' && curr->child != nullptr)
    {
        std::cout << "rmdir: " << path << ": Directory not empty" << std::endl;
        return;
    }
    if (prev == nullptr)
    {
        dir->child = curr->link;
    }
    else
    {
        prev->link = curr->link;
    }
    delete curr;
    cout << "rm: removed '" << path << "'" << endl;
}

void dupl(TreeNode *root, TreeNode *pwd, string src, string dst, int keep)
{
    string *src_paths = split_name(src);
    string *dst_paths = split_name(dst);
    TreeNode *src_dir = cd(root, pwd, src_paths[0]);
    TreeNode *dst_dir = cd(root, pwd, dst_paths[0]);
    if (src_dir == nullptr || dst_dir == nullptr)
    {
        return;
    }
    TreeNode *src_node = find_on_pwd(src_dir->child, src_paths[1]);
    if (src_node == nullptr)
    {
        std::cout << "cp: " << src << ": No such file or directory" << std::endl;
        return;
    }
    if (src_node->type == 'd')
    {
        std::cout << "cp: omitting directory '" << src << "'" << std::endl;
        return;
    }
    TreeNode *dst_node = find_on_pwd(dst_dir->child, dst_paths[1]);
    if (dst_node != nullptr)
    {
        std::cout << "cp: cannot overwrite non-directory '" << dst << "' with directory '" << src << "'" << std::endl;
        return;
    }
    TreeNode *newNode = new TreeNode(dst_dir, dst_paths[1]);
    newNode->type = src_node->type;
    newNode->contents = src_node->contents;
    newNode->permission = src_node->permission;
    newNode->cdate = src_node->cdate;
    newNode->mdate = src_node->mdate;
    newNode->link = dst_dir->child;
    dst_dir->child = newNode;
    if (keep == 0)
    {
        remove(root, pwd, src);
    }
    cout << "cp: copied '" << src << "' to '" << dst << "'" << endl;
}

void edit(TreeNode *root, TreeNode *pwd, string path)
{
    TreeNode *file = find_node(root, pwd, path);
    if (file == nullptr)
    {
        std::cout << "edit: " << path << ": No such file or directory" << std::endl;
        return;
    }
    std::cout << "Enter new contents for " << path << " (end with an empty line):" << std::endl;
    file->contents.clear();
    std::string line;
    while (std::getline(std::cin, line))
    {
        if (line.empty())
        {
            break;
        }
        file->contents.push_back(line);
    }
    file->mdate = curr_time();
    cout << "edit: updated contents of '" << path << "'" << endl;
}

void cat(TreeNode *root, TreeNode *pwd, string path)
{
    TreeNode *file = find_node(root, pwd, path);
    if (file == nullptr)
    {
        std::cout << "cat: " << path << ": No such file or directory" << std::endl;
        return;
    }
    for (const std::string &line : file->contents)
    {
        std::cout << line << std::endl;
    }
}

void chmod(TreeNode *root, TreeNode *pwd, string path, string new_modes)
{
    TreeNode *file = find_node(root, pwd, path);
    if (file == nullptr)
    {
        std::cout << "chmod: " << path << ": No such file or directory" << std::endl;
        return;
    }
    try
    {
        int new_perm = std::stoi(new_modes, nullptr, 8);
        file->permission = new_perm;
        file->mdate = curr_time();
        cout << "chmod: updated permissions of '" << path << "'" << endl;
    }
    catch (const std::exception &e)
    {
        std::cout << "chmod: invalid mode: " << new_modes << std::endl;
    }
}

void clear_screen()
{
#if defined(_WIN32) || defined(_WIN64)
    system("cls");
#else
    system("clear");
#endif
}

string curr_time()
{
    std::time_t t = std::time(nullptr);
    std::tm tm = *std::localtime(&t);
    std::ostringstream oss;
    oss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return oss.str();
}

int compileFile(TreeNode *root, TreeNode *pwd, string path)
{
    TreeNode *file = find_node(root, pwd, path);

    if (!file)
    {
        cout << "compile: file not found\n";
        return -1;
    }

    ofstream out("temp.cpp");

    for (auto &line : file->contents)
    {
        out << line << "\n";
    }
    out.close();

    int result = system("g++ temp.cpp -o temp.exe");

    if (result == 0)
    {
        cout << "Compilation successful\n";
        return 0;
    }
    else
    {
        cout << "Compilation failed\n";
        return -1;
    }
}
void runFile(TreeNode *root, TreeNode *pwd, string path)
{
    int status = compileFile(root, pwd, path);

    if (status != 0)
    {
        cout << "Run aborted due to compilation error\n";
        return;
    }

    cout << "Running program...\n";
    system("temp.exe");   // ✅ FIXED
}

void collectFiles(TreeNode* node, string path,
                  vector<pair<string,string>>& files)
{
    if (!node) return;

    string fullPath = path + "/" + node->name;

    if (node->type == '-')  // file
    {
        string content = "";
        for (auto &line : node->contents)
            content += line + " ";

        files.push_back({fullPath, content});
    }

    collectFiles(node->child, fullPath, files);
    collectFiles(node->link, path, files);
}

string buildJSON(vector<pair<string,string>>& files, string query)
{
    string json = "{ \"query\": \"" + query + "\", \"files\": [";

    for (int i = 0; i < files.size(); i++)
    {
        json += "{ \"path\": \"" + files[i].first +
                "\", \"content\": \"" + files[i].second + "\" }";

        if (i != files.size() - 1)
            json += ",";
    }

    json += "]}";
    return json;
}

string sendRequest(string jsonData)
{
    // 🔹 write JSON to file
    ofstream out("input.json");
    out << jsonData;
    out.close();

    // 🔹 call python script
    system("python client.py");

    // 🔹 read response
    ifstream in("output.txt");
    string response((istreambuf_iterator<char>(in)),
                     istreambuf_iterator<char>());

    return response;
}

void smartFind(TreeNode* root, string query)
{
    vector<pair<string,string>> files;

    collectFiles(root->child, "", files);

    if (files.empty())
    {
        cout << "No files found\n";
        return;
    }

    string jsonData = buildJSON(files, query);

    string response = sendRequest(jsonData);

    // 🔥 CLEAN OUTPUT
    cout << "\nBest Matches:\n";

    int rank = 1;
    size_t pos = 0;

    while ((pos = response.find("\"path\"")) != string::npos)
    {
        size_t start = response.find("\"", pos + 7);
        if (start == string::npos) break;
        start++;

        size_t end = response.find("\"", start);
        if (end == string::npos) break;

        string path = response.substr(start, end - start);

        size_t scorePos = response.find("\"score\"", end);
        if (scorePos == string::npos) break;

        size_t scoreStart = response.find(":", scorePos);
        if (scoreStart == string::npos) break;
        scoreStart++;

        size_t scoreEnd = response.find(",", scoreStart);
        if (scoreEnd == string::npos)
            scoreEnd = response.find("}", scoreStart);

        if (scoreEnd == string::npos) break;

        string score = response.substr(scoreStart, scoreEnd - scoreStart);

        // 🔥 CLEAN EXTRA CHARACTERS
        score.erase(remove(score.begin(), score.end(), '}'), score.end());
        score.erase(remove(score.begin(), score.end(), ']'), score.end());

        cout << rank++ << ". " << path << " (Score:" << score << ")\n";

        response = response.substr(scoreEnd);
    }
}