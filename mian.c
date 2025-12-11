#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NAME_LEN 50
#define FILE_NAME "accounts.csv"

// 账户结构体（链表节点）
typedef struct Account {
    int id;                         // 账户ID
    char name[NAME_LEN];            // 户名
    unsigned long password_hash;    // 密码哈希值
    double balance;                 // 余额
    struct Account *next;           // 指向下一个账户
} Account;

Account *head = NULL;               // 链表头指针
int nextAccountId = 1;              // 下一个可用账户ID

// 函数声明
void showMenu();
void loadAccountsFromFile();
void saveAccountsToFile();

void createAccount();
void deposit();
void withdrawMoney();
void transferMoney();
void queryAccount();
void listAllAccounts();

Account* findAccountById(int id);
int verifyPassword(Account *acc);

void clearInputBuffer();
unsigned long hashPassword(const char *str);

// 主函数
int main() {
    int choice;

    // 程序启动时，从CSV加载账户数据
    loadAccountsFromFile();

    while (1) {
        showMenu();
        printf("请输入功能编号: ");
        if (scanf("%d", &choice) != 1) {
            printf("输入无效，请输入数字。\n");
            clearInputBuffer();
            continue;
        }

        switch (choice) {
            case 1:
                createAccount();
                break;
            case 2:
                deposit();
                break;
            case 3:
                withdrawMoney();
                break;
            case 4:
                transferMoney();
                break;
            case 5:
                queryAccount();
                break;
            case 6:
                listAllAccounts();
                break;
            case 0:
                printf("已退出系统。\n");
                // 退出前保存一次（理论上每次操作已保存，这里只是保险）
                saveAccountsToFile();
                // 释放链表内存
                {
                    Account *p = head;
                    while (p) {
                        Account *tmp = p;
                        p = p->next;
                        free(tmp);
                    }
                }
                return 0;
            default:
                printf("无效选项，请重新输入。\n");
        }
    }

    return 0;
}

// 显示菜单
void showMenu() {
    printf("\n");
    printf("========== 银行账户管理系统（链表 + CSV + 哈希密码） ==========\n");
    printf("1. 新建账户\n");
    printf("2. 存款（需密码）\n");
    printf("3. 取款（需密码）\n");
    printf("4. 转账（需密码）\n");
    printf("5. 查询账户（需密码）\n");
    printf("6. 显示所有账户（不需密码）\n");
    printf("0. 退出并保存\n");
    printf("==============================================================\n");
}

// 清空输入缓冲区
void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {}
}

// 简单的字符串哈希函数（djb2 算法）
// 实际安全场景下请使用真正的密码哈希算法（如 SHA-256 + salt）
unsigned long hashPassword(const char *str) {
    unsigned long hash = 5381;
    int c;
    while ((c = (unsigned char)*str++)) {
        hash = ((hash << 5) + hash) + c;  // hash * 33 + c
    }
    return hash;
}

// 从CSV文件加载账户
void loadAccountsFromFile() {
    FILE *fp = fopen(FILE_NAME, "r");
    if (fp == NULL) {
        // 文件不存在，视为无账户
        printf("未找到账户文件 %s ，将从空数据开始。\n", FILE_NAME);
        return;
    }

    char line[256];
    int maxId = 0;
    while (fgets(line, sizeof(line), fp)) {
        // 期望格式：id,name,password_hash,balance
        char *token;
        Account *acc = (Account*)malloc(sizeof(Account));
        if (!acc) {
            printf("内存分配失败！\n");
            fclose(fp);
            return;
        }

        // id
        token = strtok(line, ",");
        if (!token) { free(acc); continue; }
        acc->id = atoi(token);

        // name
        token = strtok(NULL, ",");
        if (!token) { free(acc); continue; }
        strncpy(acc->name, token, NAME_LEN - 1);
        acc->name[NAME_LEN - 1] = '\0';

        // password_hash
        token = strtok(NULL, ",");
        if (!token) { free(acc); continue; }
        acc->password_hash = strtoul(token, NULL, 10);

        // balance
        token = strtok(NULL, ",\n");
        if (!token) { free(acc); continue; }
        acc->balance = atof(token);

        // 插入到链表尾部
        acc->next = NULL;
        if (head == NULL) {
            head = acc;
        } else {
            Account *p = head;
            while (p->next != NULL) {
                p = p->next;
            }
            p->next = acc;
        }

        if (acc->id > maxId)
            maxId = acc->id;
    }

    nextAccountId = maxId + 1;

    fclose(fp);
    printf("已从 %s 加载账户数据。\n", FILE_NAME);
}

// 保存账户到CSV文件
void saveAccountsToFile() {
    FILE *fp = fopen(FILE_NAME, "w");
    if (fp == NULL) {
        printf("保存失败：无法打开文件 %s\n", FILE_NAME);
        return;
    }

    Account *p = head;
    while (p != NULL) {
        // id,name,password_hash,balance
        fprintf(fp, "%d,%s,%lu,%.2f\n",
                p->id, p->name, p->password_hash, p->balance);
        p = p->next;
    }

    fclose(fp);
    printf("账户信息已保存到 %s\n", FILE_NAME);
}

// 新建账户
void createAccount() {
    Account *acc = (Account*)malloc(sizeof(Account));
    if (!acc) {
        printf("内存分配失败，无法创建账户。\n");
        return;
    }

    acc->id = nextAccountId++;

    printf("请输入户名(不含空格): ");
    scanf("%s", acc->name);

    char pwd[100];
    printf("请设置密码(不含空格): ");
    scanf("%99s", pwd);

    acc->password_hash = hashPassword(pwd);
    acc->balance = 0.0;
    acc->next = NULL;

    // 插入链表尾部
    if (head == NULL) {
        head = acc;
    } else {
        Account *p = head;
        while (p->next != NULL) {
            p = p->next;
        }
        p->next = acc;
    }

    printf("账户创建成功！账户ID: %d, 户名: %s, 初始余额: %.2f\n",
           acc->id, acc->name, acc->balance);

    // 保存到文件
    saveAccountsToFile();
}

// 查找账户（按ID）
Account* findAccountById(int id) {
    Account *p = head;
    while (p != NULL) {
        if (p->id == id) return p;
        p = p->next;
    }
    return NULL;
}

// 验证密码，正确返回1，错误返回0
int verifyPassword(Account *acc) {
    char inputPwd[100];
    printf("请输入账户密码: ");
    scanf("%99s", inputPwd);

    unsigned long h = hashPassword(inputPwd);
    if (h == acc->password_hash) {
        return 1;
    } else {
        printf("密码错误！\n");
        return 0;
    }
}

// 存款（需密码）
void deposit() {
    int id;
    double amount;

    printf("请输入要存款的账户ID: ");
    if (scanf("%d", &id) != 1) {
        printf("输入无效。\n");
        clearInputBuffer();
        return;
    }

    Account *acc = findAccountById(id);
    if (acc == NULL) {
        printf("未找到该账户！\n");
        return;
    }

    if (!verifyPassword(acc)) {
        return;
    }

    printf("请输入存款金额: ");
    if (scanf("%lf", &amount) != 1) {
        printf("输入无效。\n");
        clearInputBuffer();
        return;
    }

    if (amount <= 0) {
        printf("存款金额必须大于0！\n");
        return;
    }

    acc->balance += amount;
    printf("存款成功！账户ID: %d, 户名: %s, 当前余额: %.2f\n",
           acc->id, acc->name, acc->balance);

    saveAccountsToFile();
}

// 取款（需密码）
void withdrawMoney() {
    int id;
    double amount;

    printf("请输入要取款的账户ID: ");
    if (scanf("%d", &id) != 1) {
        printf("输入无效。\n");
        clearInputBuffer();
        return;
    }

    Account *acc = findAccountById(id);
    if (acc == NULL) {
        printf("未找到该账户！\n");
        return;
    }

    if (!verifyPassword(acc)) {
        return;
    }

    printf("请输入取款金额: ");
    if (scanf("%lf", &amount) != 1) {
        printf("输入无效。\n");
        clearInputBuffer();
        return;
    }

    if (amount <= 0) {
        printf("取款金额必须大于0！\n");
        return;
    }

    if (amount > acc->balance) {
        printf("余额不足！当前余额: %.2f\n", acc->balance);
        return;
    }

    acc->balance -= amount;
    printf("取款成功！账户ID: %d, 户名: %s, 当前余额: %.2f\n",
           acc->id, acc->name, acc->balance);

    saveAccountsToFile();
}

// 转账（需密码，仅验证转出账户密码）
void transferMoney() {
    int fromId, toId;
    double amount;

    printf("请输入转出账户ID: ");
    if (scanf("%d", &fromId) != 1) {
        printf("输入无效。\n");
        clearInputBuffer();
        return;
    }
    printf("请输入转入账户ID: ");
    if (scanf("%d", &toId) != 1) {
        printf("输入无效。\n");
        clearInputBuffer();
        return;
    }

    if (fromId == toId) {
        printf("转出账户和转入账户不能相同！\n");
        return;
    }

    Account *fromAcc = findAccountById(fromId);
    Account *toAcc   = findAccountById(toId);

    if (fromAcc == NULL) {
        printf("未找到转出账户！\n");
        return;
    }
    if (toAcc == NULL) {
        printf("未找到转入账户！\n");
        return;
    }

    // 验证转出账户密码
    if (!verifyPassword(fromAcc)) {
        return;
    }

    printf("请输入转账金额: ");
    if (scanf("%lf", &amount) != 1) {
        printf("输入无效。\n");
        clearInputBuffer();
        return;
    }

    if (amount <= 0) {
        printf("转账金额必须大于0！\n");
        return;
    }

    if (amount > fromAcc->balance) {
        printf("余额不足！转出账户当前余额: %.2f\n", fromAcc->balance);
        return;
    }

    fromAcc->balance -= amount;
    toAcc->balance   += amount;

    printf("转账成功！\n");
    printf("转出账户(ID: %d, 户名: %s) 当前余额: %.2f\n",
           fromAcc->id, fromAcc->name, fromAcc->balance);
    printf("转入账户(ID: %d, 户名: %s) 当前余额: %.2f\n",
           toAcc->id, toAcc->name, toAcc->balance);

    saveAccountsToFile();
}

// 查询账户（需密码）
void queryAccount() {
    int id;
    printf("请输入要查询的账户ID: ");
    if (scanf("%d", &id) != 1) {
        printf("输入无效。\n");
        clearInputBuffer();
        return;
    }

    Account *acc = findAccountById(id);
    if (acc == NULL) {
        printf("未找到该账户！\n");
        return;
    }

    if (!verifyPassword(acc)) {
        return;
    }

    printf("账户信息如下：\n");
    printf("账户ID: %d\n", acc->id);
    printf("户名: %s\n", acc->name);
    printf("余额: %.2f\n", acc->balance);
}

// 显示所有账户（不需密码，一般调试用）
void listAllAccounts() {
    if (head == NULL) {
        printf("目前没有任何账户。\n");
        return;
    }

    printf("所有账户列表：\n");
    printf("ID\t户名\t\t余额\n");
    printf("---------------------------------------\n");

    Account *p = head;
    while (p != NULL) {
        printf("%d\t%-10s\t%.2f\n",
               p->id, p->name, p->balance);
        p = p->next;
    }
}
