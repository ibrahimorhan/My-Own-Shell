#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h> 
#include <iostream>
#include <sstream>
#include <fcntl.h>
using namespace std;
void rearrange(string arr[15],string command){ // this function is for footprint command
	if(!arr[0].empty()){                       // it takes an array as a parameter and the command
		string temp=arr[0];                    // when a new command is taken it replaces the commands 
		arr[0]=command;                        //and put next command in the arr[0]
		string temp2;
		for(int i =1;i<15;i++){
			if(!arr[i].empty()){
				temp2=arr[i];
				arr[i]=temp;
				temp=temp2;
			}else{
				arr[i]=temp;
				break;
			}
			
		}
	}else{
		arr[0]=command;
	}
	
}
int main()
{
 cout<<" Welcome"<<endl;    // shell starts with a welcome message
 string cmdList[15];		// declaration of command array
 string cmd;				
 string name=getenv("USER");// it is used for the get username 
 cout<<name;
 cout<<" >>> ";
 getline(cin, cmd);			//first command is taken
 int pid;					
 while(cmd.compare("exit")){// shell works until an exit command typed
 	 rearrange(cmdList,cmd);// when a new command is typed, command array is reaarenged
 	 stringstream s(cmd);	// to take input properly, command line is splitted into the words
	 string word;			// used stringstream and put the words into the words array
	 string words[10];		// I made an assumption that line can't contain more than 10 words
	 int i=0;
	 while(s>>word){
	 	words[i]=word;
	 	i++;
	 }
	 if((!words[0].compare("listdir"))&&(i==1)){ // From now on all command compared with our command set in if else block
	  pid=fork();			// a child process is created
	  if(pid==0){			
	   execlp("ls","ls",(char*)NULL);	// in child process ls command program is executed
	  }else if(pid>0){
	   wait(0);			// parent waited until a change happened in child process
	  }else{
	   cout<<"Error in fork"; 
	  }
	 }else if((!words[0].compare("listdir"))&&(!words[1].compare("-a"))&&(i==2)) { // this is the if block for listdir -a command
	 	pid=fork();			// a child process is created
	 	if(pid==0){
	 		execlp("ls","ls","-a",(char*)NULL);		// in child process ls -a command program is executed. -a is given as another argument
	 	}else if(pid>0){
	 		wait(0);		// parent waited until a change happened in child process
	 	}else{
	 		cout<<"Error in fork";
	 	}


	 }else if((!words[0].compare("currentpath"))&&(i==1)){	// this is the if block for currentpath command
	 	pid=fork();		// a child process is created
	 	if(pid==0){
	 		execlp("pwd","pwd",(char*)NULL);	// in child process pwd command program is executed
	 	}else if(pid>0){
	 		wait(0);		//parent process waited until child process ended
	 	}else{
	 		cout<<"Error in fork";
	 	}
	 }else if((!words[0].compare("printfile"))&&(i==2)) { // this is the if block for printfile
	 	pid=fork();// a child process is created
	 	if(pid==0){
	 		string a=words[1];	//file name taken as string variable
	 		execlp("cat","cat",a.c_str(),(char*)NULL); // it is turned to a string
	 	}else if(pid>0){
	 		wait(0);	//parent process waited until child process ended
	 	}else{
	 		cout<<"Error in fork";
	 	}
	 }else if((!words[0].compare("printfile"))&&(i!=2)){// this is the if block for printfile "file1">"file2"
	 	pid=fork();// a child process is created
	 	if(pid==0){
	 		string a=words[1];	//first and second file name is taken
	 		string b=words[3];
	 		int ofd = creat(b.c_str(), 0644);	//file descriptor of file 2
	 		dup2(ofd,1);		// write part of descriptor of file2 is directed to new descriptor
	 		execlp("cat","cat",a.c_str(),(char*)NULL);	// cat command is executed with file1
	 	}else if(pid>0){
	 		wait(0);	//parent process waited until child process ended
	 	}else{
	 		cout<<"Error in fork";
	 	}
	 }else if((!words[0].compare("listdir"))&&(!words[1].compare("|"))){
	 	string searched;
	 	int count=0;
	 	for(int i =0;i<cmd.length()-1;i++){ // this for is to get argument without ""
	 		if(cmd[i-1]=='"'){
	 			count++;
	 		}
	 		if(count==1){
	 			searched+=cmd[i];
	 		}
	 	}
	 	int pipefd[2];	//new pipe is created
	 	pipe(pipefd);
	 	pid=fork(); // child process is created
	 	if(pid==0){
	 		dup2(pipefd[1],1);	//file descripter is redirected
	 		close(pipefd[0]);	//all ends of pipe is closed when out job is done
	 		close(pipefd[1]);			
	 		execlp("ls","ls",(char*)NULL); // ls is executed now ls is writing to pipe
	 		
	 	}else if(pid>0){
	 		pid=fork();		//another child is created
	 		if(pid==0){
	 			dup2(pipefd[0],0);	// file descriptor is redirected
	 			close(pipefd[0]);//all ends of pipe is closed when out job is done
	 			close(pipefd[1]);
	 			execlp("grep","grep",searched.c_str(),(char*)NULL);// grep is reading from the pipe
	 			
	 		}else{// parent process
	 			close(pipefd[0]);//all ends of pipe is closed when out job is done
	 			close(pipefd[1]);
	 			wait(0); // parent waits for the children
	 		}
	 		wait(0);
	 	}
	 }else if((!words[0].compare("listdir"))&&(!words[2].compare("|"))){// listdir -a | grep "argument"
	 	string searched; 
	 	int count=0;
	 	for(int i =0;i<cmd.length()-1;i++){ // this for is to get argument without ""
	 		if(cmd[i-1]=='"'){
	 			count++;
	 		}
	 		if(count==1){
	 			searched+=cmd[i];
	 		}
	 	}
	 	int pipefd[2]; //new pipe is created
	 	pipe(pipefd);
	 	pid=fork();
	 	if(pid==0){
	 		dup2(pipefd[1],1); // file descriptor is redirected
	 		close(pipefd[0]);//all ends of pipe is closed when out job is done
	 		close(pipefd[1]);			
	 		execlp("ls","ls","-a",(char*)NULL);// ls is executed now ls is writing to pipe
	 		
	 	}else if(pid>0){
	 		pid=fork();
	 		if(pid==0){
	 			dup2(pipefd[0],0);
	 			close(pipefd[0]);//all ends of pipe is closed when out job is done
	 			close(pipefd[1]);
	 			execlp("grep","grep",searched.c_str(),(char*)NULL);// grep is reading from the pipe
	 			
	 		}else{
	 			close(pipefd[0]);//all ends of pipe is closed when out job is done
	 			close(pipefd[1]);
	 			wait(0);
	 		}
	 	}
	 }else if(!words[0].compare("footprint")){ // this is for footprint command
	 	for(int i =0;i<15;i++){
	 		if(!cmdList[14-i].empty()){ //starts printing from the last
	 			cout<<cmdList[14-i]<<endl;
	 			}
	 		
	 	}
	 }
	 string name=getenv("USER");
 	 cout<<name;
 	 cout<<" >>> ";
	 getline(cin, cmd); // get next command
 }
 
  

 return 0;
}

