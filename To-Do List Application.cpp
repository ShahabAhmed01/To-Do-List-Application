#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <ctime>
#include <iomanip>
#include <cstdlib>

#ifdef _WIN32
    #include <windows.h>
#endif

using namespace std;

// ==================== ANSI COLOR CODES ====================
#ifdef _WIN32
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    #define RESET_COLOR SetConsoleTextAttribute(hConsole, 7)
    #define BLUE SetConsoleTextAttribute(hConsole, 9)
    #define GREEN SetConsoleTextAttribute(hConsole, 10)
    #define CYAN SetConsoleTextAttribute(hConsole, 11)
    #define RED SetConsoleTextAttribute(hConsole, 12)
    #define YELLOW SetConsoleTextAttribute(hConsole, 14)
    #define PURPLE SetConsoleTextAttribute(hConsole, 13)
    #define GRAY SetConsoleTextAttribute(hConsole, 8)
#else
    #define RESET_COLOR "\033[0m"
    #define BLUE "\033[94m"
    #define GREEN "\033[92m"
    #define CYAN "\033[96m"
    #define RED "\033[91m"
    #define YELLOW "\033[93m"
    #define PURPLE "\033[95m"
    #define GRAY "\033[90m"
    #define BOLD "\033[1m"
#endif

// ==================== TASK STRUCTURE ====================
struct Task {
    int id;
    string description;
    bool completed;
    time_t createdDate;

    Task(int taskId, string desc)
        : id(taskId), description(desc), completed(false) {
        time(&createdDate);
    }
};

// ==================== FUNCTION PROTOTYPES ====================
void clearScreen();
void pauseConsole();
void printHeader();
void printCreatorInfo();
void printCentered(const string& text, const string& fill = " ");
string formatDate(time_t t);
void printTask(const Task& task);
void printProgressBar(int completed, int total);
bool readInt(const string& prompt, int& value);
bool isBlank(const string& text);

// Main functions - Only the requested ones
void displayMainMenu();
void addTask(vector<Task>& tasks, int& nextId);
void viewTasks(const vector<Task>& tasks);
void removeTask(vector<Task>& tasks);

// ==================== UTILITY FUNCTIONS ====================
void clearScreen() {
    #ifdef _WIN32
        system("cls");
    #else
        cout << "\033[2J\033[H" << flush;
    #endif
}

#ifdef _WIN32
void setColor(int color) {
    SetConsoleTextAttribute(hConsole, static_cast<WORD>(color));
}
#endif

void pauseConsole() {
    cout << "\n  Press Enter to continue...";
    string line;
    getline(cin, line);
}

bool isBlank(const string& text) {
    return text.find_first_not_of(" \t\r\n") == string::npos;
}

bool readInt(const string& prompt, int& value) {
    while (true) {
        cout << prompt;
        string line;
        if (!getline(cin, line)) {
            return false;
        }

        if (isBlank(line)) {
            cout << "  Invalid input. Please enter a number.\n";
            continue;
        }

        size_t start = line.find_first_not_of(" \t\r\n");
        size_t end = line.find_last_not_of(" \t\r\n");
        string token = line.substr(start, end - start + 1);

        try {
            size_t consumed = 0;
            int parsed = stoi(token, &consumed);
            if (consumed == token.size()) {
                value = parsed;
                return true;
            }
        } catch (...) {
        }

        cout << "  Invalid input. Please enter a number.\n";
    }
}

size_t utf8Length(const string& text) {
    size_t length = 0;
    for (unsigned char c : text) {
        if ((c & 0xC0) != 0x80) {
            ++length;
        }
    }
    return length;
}

void printCentered(const string& text, const string& fill) {
    const int width = 60;
    const int textWidth = static_cast<int>(utf8Length(text));
    int padding = (width - textWidth) / 2;
    if (padding < 0) padding = 0;

    #ifndef _WIN32
        cout << BOLD;
    #endif

    for (int i = 0; i < padding; ++i) cout << fill;
    cout << text;
    for (int i = 0; i < padding; ++i) cout << fill;
    cout << endl;

    #ifndef _WIN32
        cout << RESET_COLOR;
    #endif
}

void printHeader() {
    clearScreen();
    #ifndef _WIN32
        cout << PURPLE << BOLD;
    #else
        setColor(13);
    #endif

    cout << "\n";
    printCentered("══════════════════════════════════════", "═");
    printCentered("     TASKFLOW - MINIMAL TO-DO LIST     ");
    printCentered("══════════════════════════════════════", "═");

    #ifndef _WIN32
        cout << CYAN;
    #else
        setColor(11);
    #endif

    time_t now = time(0);
    cout << "\n";
    printCentered("Date: " + formatDate(now));

    #ifdef _WIN32
        RESET_COLOR;
    #else
        cout << RESET_COLOR;
    #endif
}

void printCreatorInfo() {
    #ifndef _WIN32
        cout << GRAY << BOLD;
    #else
        setColor(8);
    #endif

    cout << "\n";
    printCentered("──────────────────────────────────────", "─");
    printCentered("Creator: Shahab Ahmed");
    printCentered("LinkedIn: linkedin.com/in/shahabahmed01");
    printCentered("──────────────────────────────────────", "─");

    #ifdef _WIN32
        RESET_COLOR;
    #else
        cout << RESET_COLOR;
    #endif
    cout << "\n";
}

string formatDate(time_t t) {
    tm timeinfo{};

    #ifdef _WIN32
        if (localtime_s(&timeinfo, &t) != 0) {
            return "unknown";
        }
    #else
        tm* result = localtime(&t);
        if (result == nullptr) {
            return "unknown";
        }
        timeinfo = *result;
    #endif

    char buffer[80];
    strftime(buffer, 80, "%Y-%m-%d %H:%M", &timeinfo);
    return string(buffer);
}

void printTask(const Task& task) {
    #ifndef _WIN32
        if (task.completed) {
            cout << GREEN << "  [x] ";
        } else {
            cout << YELLOW << "  [ ] ";
        }
    #else
        if (task.completed) {
            setColor(10);
            cout << "  [x] ";
        } else {
            setColor(14);
            cout << "  [ ] ";
        }
    #endif

    cout << "ID: " << task.id;

    #ifndef _WIN32
        cout << RESET_COLOR;
    #else
        RESET_COLOR;
    #endif

    cout << " | " << task.description;

    #ifndef _WIN32
        cout << GRAY;
    #else
        setColor(8);
    #endif

    cout << "  (Created: " << formatDate(task.createdDate) << ")" << endl;

    #ifndef _WIN32
        cout << RESET_COLOR;
    #else
        RESET_COLOR;
    #endif
}

void printProgressBar(int completed, int total) {
    if (total == 0) return;

    float percentage = (float)completed / total * 100;
    int barWidth = 25;
    int filledWidth = static_cast<int>((percentage / 100.0f) * barWidth);

    #ifndef _WIN32
        cout << CYAN;
    #else
        setColor(11);
    #endif

    cout << "\n  Progress: [";

    for (int i = 0; i < barWidth; i++) {
        if (i < filledWidth) {
            #ifndef _WIN32
                cout << GREEN << "█";
            #else
                setColor(10); cout << "█";
            #endif
        } else {
            #ifndef _WIN32
                cout << GRAY << "░";
            #else
                setColor(8); cout << "░";
            #endif
        }
    }

    #ifndef _WIN32
        cout << CYAN << "] " << setprecision(0) << fixed << percentage << "%" << RESET_COLOR << endl;
    #else
        setColor(11); cout << "] " << setprecision(0) << fixed << percentage << "%" << endl;
        RESET_COLOR;
    #endif
}

// ==================== MAIN FUNCTIONS ====================
void displayMainMenu() {
    printHeader();

    #ifndef _WIN32
        cout << BLUE << BOLD;
    #else
        setColor(9);
    #endif

    cout << "\n";
    printCentered("─────────── MAIN MENU ───────────", "─");

    #ifndef _WIN32
        cout << CYAN;
    #else
        setColor(11);
    #endif

    cout << "\n  1. Add New Task\n";
    cout << "  2. View All Tasks\n";
    cout << "  3. Mark Task as Completed\n";
    cout << "  4. Remove Completed Tasks\n";
    cout << "  5. About / Creator Info\n";
    cout << "  0. Exit\n";

    #ifndef _WIN32
        cout << BLUE;
    #else
        setColor(9);
    #endif

    cout << "\n";
    printCentered("──────────────────────────────────", "─");
}

void addTask(vector<Task>& tasks, int& nextId) {
    printHeader();

    #ifndef _WIN32
        cout << GREEN << BOLD;
    #else
        setColor(10);
    #endif

    cout << "\n";
    printCentered("+ ADD NEW TASK");
    cout << endl;

    string description;
    cout << "  Enter task description: ";
    if (!getline(cin, description)) {
        return;
    }

    if (isBlank(description)) {
        #ifndef _WIN32
            cout << RED;
        #else
            setColor(12);
        #endif
        cout << "\n  Task description cannot be empty!\n";
        #ifdef _WIN32
            RESET_COLOR;
        #else
            cout << RESET_COLOR;
        #endif
        pauseConsole();
        return;
    }

    tasks.push_back(Task(nextId, description));

    #ifndef _WIN32
        cout << GREEN;
    #else
        setColor(10);
    #endif

    cout << "\n  Task added successfully! (ID: " << nextId << ")\n";
    nextId++;

    #ifdef _WIN32
        RESET_COLOR;
    #else
        cout << RESET_COLOR;
    #endif
    pauseConsole();
}

void viewTasks(const vector<Task>& tasks) {
    printHeader();

    if (tasks.empty()) {
        #ifndef _WIN32
            cout << YELLOW << BOLD;
        #else
            setColor(14);
        #endif
        cout << "\n";
        printCentered("NO TASKS FOUND");
        cout << "\n  Your task list is empty. Add some tasks to get started!\n";
        #ifdef _WIN32
            RESET_COLOR;
        #else
            cout << RESET_COLOR;
        #endif
        pauseConsole();
        return;
    }

    #ifndef _WIN32
        cout << CYAN << BOLD;
    #else
        setColor(11);
    #endif

    cout << "\n";
    printCentered("YOUR TASKS");
    cout << endl;

    int completed = 0;
    for (const auto& task : tasks) {
        printTask(task);
        if (task.completed) completed++;
    }

    printProgressBar(completed, static_cast<int>(tasks.size()));

    #ifndef _WIN32
        cout << GRAY;
    #else
        setColor(8);
    #endif

    cout << "\n  Summary: ";
    cout << "Total: " << tasks.size()
         << " | Completed: " << completed
         << " | Pending: " << tasks.size() - completed << endl;

    #ifdef _WIN32
        RESET_COLOR;
    #else
        cout << RESET_COLOR;
    #endif
    pauseConsole();
}

void markTaskComplete(vector<Task>& tasks) {
    printHeader();

    if (tasks.empty()) {
        #ifndef _WIN32
            cout << RED;
        #else
            setColor(12);
        #endif
        cout << "  No tasks to mark as complete!\n";
        #ifdef _WIN32
            RESET_COLOR;
        #else
            cout << RESET_COLOR;
        #endif
        pauseConsole();
        return;
    }

    viewTasks(tasks);

    int taskId;
    if (!readInt("\n  Enter Task ID to mark as complete (0 to cancel): ", taskId)) {
        return;
    }

    if (taskId == 0) return;

    for (auto& task : tasks) {
        if (task.id == taskId) {
            if (!task.completed) {
                task.completed = true;
                #ifndef _WIN32
                    cout << GREEN;
                #else
                    setColor(10);
                #endif
                cout << "\n  Task marked as complete!\n";
            } else {
                #ifndef _WIN32
                    cout << YELLOW;
                #else
                    setColor(14);
                #endif
                cout << "\n  Task is already completed!\n";
            }
            #ifdef _WIN32
                RESET_COLOR;
            #else
                cout << RESET_COLOR;
            #endif
            pauseConsole();
            return;
        }
    }

    #ifndef _WIN32
        cout << RED;
    #else
        setColor(12);
    #endif
    cout << "\n  Task with ID " << taskId << " not found!\n";
    #ifdef _WIN32
        RESET_COLOR;
    #else
        cout << RESET_COLOR;
    #endif
    pauseConsole();
}

void removeTask(vector<Task>& tasks) {
    printHeader();

    if (tasks.empty()) {
        #ifndef _WIN32
            cout << RED;
        #else
            setColor(12);
        #endif
        cout << "  No tasks to remove!\n";
        #ifdef _WIN32
            RESET_COLOR;
        #else
            cout << RESET_COLOR;
        #endif
        pauseConsole();
        return;
    }

    viewTasks(tasks);

    cout << "\n  Choose removal option:\n";
    cout << "  1. Remove specific completed task by ID\n";
    cout << "  2. Remove all completed tasks\n";
    cout << "  0. Cancel\n";

    int option;
    if (!readInt("  Enter option: ", option)) {
        return;
    }

    if (option == 0) return;

    if (option == 1) {
        int taskId;
        if (!readInt("  Enter Task ID to remove: ", taskId)) {
            return;
        }

        auto it = find_if(tasks.begin(), tasks.end(),
                        [taskId](const Task& t) { return t.id == taskId; });

        if (it != tasks.end()) {
            if (it->completed) {
                cout << "\n  Removing task: " << it->description << endl;
                tasks.erase(it);
                #ifndef _WIN32
                    cout << GREEN;
                #else
                    setColor(10);
                #endif
                cout << "  Task removed successfully!\n";
            } else {
                #ifndef _WIN32
                    cout << YELLOW;
                #else
                    setColor(14);
                #endif
                cout << "\n  Task is not completed yet. Only completed tasks can be removed.\n";
            }
        } else {
            #ifndef _WIN32
                cout << RED;
            #else
                setColor(12);
            #endif
            cout << "\n  Task with ID " << taskId << " not found!\n";
        }
    }
    else if (option == 2) {
        int initialSize = static_cast<int>(tasks.size());
        tasks.erase(remove_if(tasks.begin(), tasks.end(),
                            [](const Task& t) { return t.completed; }),
                   tasks.end());

        int removedCount = initialSize - static_cast<int>(tasks.size());
        if (removedCount > 0) {
            #ifndef _WIN32
                cout << GREEN;
            #else
                setColor(10);
            #endif
            cout << "\n  Removed " << removedCount << " completed task(s)!\n";
        } else {
            #ifndef _WIN32
                cout << YELLOW;
            #else
                setColor(14);
            #endif
            cout << "\n  No completed tasks to remove!\n";
        }
    }
    else {
        #ifndef _WIN32
            cout << YELLOW;
        #else
            setColor(14);
        #endif
        cout << "\n  Invalid removal option.\n";
    }

    #ifdef _WIN32
        RESET_COLOR;
    #else
        cout << RESET_COLOR;
    #endif
    pauseConsole();
}

// ==================== MAIN FUNCTION ====================
int main() {
    vector<Task> tasks;
    int nextId = 1;
    int choice = -1;

    // Set the Windows console to UTF-8 for the box-drawing characters.
    #ifdef _WIN32
        SetConsoleOutputCP(65001);
        SetConsoleCP(65001);
    #endif

    do {
        displayMainMenu();
        if (!readInt("  Enter your choice: ", choice)) {
            break;
        }

        switch(choice) {
            case 1:
                addTask(tasks, nextId);
                break;
            case 2:
                viewTasks(tasks);
                break;
            case 3:
                markTaskComplete(tasks);
                break;
            case 4:
                removeTask(tasks);
                break;
            case 5:
                printHeader();
                printCreatorInfo();
                pauseConsole();
                break;
            case 0:
                printHeader();
                #ifndef _WIN32
                    cout << GREEN << BOLD;
                #else
                    setColor(10);
                #endif
                printCentered("THANK YOU FOR USING TASKFLOW!");
                printCreatorInfo();
                #ifdef _WIN32
                    RESET_COLOR;
                #else
                    cout << RESET_COLOR;
                #endif
                break;
            default:
                #ifndef _WIN32
                    cout << RED;
                #else
                    setColor(12);
                #endif
                cout << "  Invalid choice! Please try again.\n";
                #ifdef _WIN32
                    RESET_COLOR;
                #else
                    cout << RESET_COLOR;
                #endif
                pauseConsole();
        }

    } while (choice != 0);

    return 0;
}
