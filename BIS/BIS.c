#include "BIS.h"
    user users[2000];
    int usercount=3;
    char name[100];
    long long int IDnumber;
    long long int account;
    double deposit;
    char password[100];
int main(){


    int userchoice=0;
    users[0]=(user){"Mouyu",13193918,2313228276,122500.6,"PASSWORD"};
    users[1]=(user){"Couvor",132345234,6868668,999999,"PASSWORD"};
    users[2]=(user){"Bydzsh",123131233,123456,114514.19,"PASSWORD"};


    printf("===============================\n");
    printf("银行信息管理系统\n");
    printf("输入数字选择对应服务\n");
    printf("1:存款\n2:取款\n3:转账\n4:查看账户信息\n5:创建账户\n6:注销账户\n0:退出\n");
    printf("===============================\n");    
        while(1){
     if(scanf("%d",&userchoice)!=1) {
        printf("输入格式错误！请重新输入\n");
        int sweeper;
        while((sweeper=getchar())!='\n'&&sweeper!=EOF);
        continue;
    }
        switch(userchoice){
            case 1: Deposit();break;
            case 2: Withdrawal();break;
            case 3: Deposit_transfer();break;
            case 4: Check_information();break;
            case 5: Add_user();break;
            case 6: Delete_user();break;
            case 0: printf("欢迎再次使用");return 0;
            default:printf("wrong input!");
        };
    }
    return 0;
}




//写以下部分的内容
//----------------------------------------------------------------------------------------
void Add_user(){//注册账户
    printf("null");
}

void Delete_user(){//注销账户
    printf("null");
}

void Check_information(){//查看账户信息 陈镜宇
    int checkflag=0;
    printf("请输入账号和密码\n");
    if(scanf("%lld %s",&account,password)!=2){//推荐大家处理输入问题时增加这样的一个检测格式是否正确和清除输入缓冲区的模块，如果输入的格式不符合要求，不仅会scanf失败，而且错误的信息会留在缓冲区里，我就犯了这么个错整出来了个死循环
         printf("输入格式错误！请重新输入\n");
         int sweeper;
         while((sweeper=getchar())!='\n'&&sweeper!=EOF);
         return;
}
    for(int i=0;i<usercount;i++){
        if(users[i].account==account&&strcmp(password,users[i].password)==0){
            printf("验证通过！以下是您账户的信息\n");
            printf("姓名:%s\n身份证号:%lld\n账号:%lld\n存款:%lf\n",users[i].name,users[i].IDnumber,users[i].account,users[i].deposit);
            checkflag=1;
        }
    }
    if(checkflag==0){
        printf("输入错误！\n");
    }
}

void Deposit(){//存款
        printf("null");
}

void Withdrawal(){//取款
        printf("null");
}

void Deposit_transfer(){//转帐
        printf("null");
}
//----------------------------------------------------------------------------------------