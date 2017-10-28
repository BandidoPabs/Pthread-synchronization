//Pablo Salinas
//COSC 3360-Operating System Fundamentals
//Assignment #3 for Summer 2017: Capulets and Montagues
//Due Date July 31, 2017

#include <iostream>
#include <string>
#include <vector>
#include <pthread.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

//Struct to hold info on the person
struct personinfo{
	string gang;
	string name;
	int atime;
	int ptime;
};



//Counter for Capulet
static int capcount = 0;

//Counter for Montague
static int moncount = 0;

//Counter for threadcount
static int pcount = 0;



//pthread locks
static pthread_mutex_t mylock;

//condition variables
static pthread_cond_t clear = PTHREAD_COND_INITIALIZER;

//Child functions prototypes
void *Capulet(void *arg);
void *Montague(void *arg);



int main(){
	struct personinfo person;
	vector<struct personinfo> ptable;
	string gtemp, ntemp;
	int atemp, ptemp;
	//initializing the lock
	pthread_mutex_init(&mylock, NULL);
	
	//storing info from file into a struct vector
	while(cin >> gtemp >> ntemp >> atemp >> ptemp){
		person.gang = gtemp;
		person.name = ntemp;
		person.atime = atemp;
		person.ptime = ptemp;
		ptable.push_back(person);
		pcount++;
		
	}

	//Declaring Thread ID && attribute
	pthread_t tid[pcount];
	pthread_attr_t attr;
	
	//Initialize and set detach attribute
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
	
	int rc;

	//creating the threads
	for(unsigned int i = 0; i < ptable.size(); i++){
		if(ptable[i].gang == "Capulet"){
			if(i != 0){
				sleep(ptable[i].atime - ptable[i-1].atime);
				rc = pthread_create(&tid[i], &attr, Capulet,(void *) &ptable[i]);
				if(rc){
					cout << "Error: return code from pthread_create() is " << rc << endl;
					exit(-1);
				}
			}
			else{
				rc = pthread_create(&tid[i], &attr, Capulet,(void *) &ptable[i]);
				if(rc){
					cout << "Error: return code from pthread_create() is " << rc << endl;
					exit(-1);
				}
			}
		}
		else if(ptable[i].gang == "Montague"){
			if(i != 0){
				sleep(ptable[i].atime - ptable[i-1].atime);
				rc = pthread_create(&tid[i], &attr, Montague,(void *) &ptable[i]);
				if(rc){
					cout << "Error: return code from pthread_create() is " << rc << endl;
					exit(-1);
				}
			}
			else{
				rc = pthread_create(&tid[i], &attr, Montague,(void *) &ptable[i]);
				if(rc){
					cout << "Error: return code from pthread_create() is " << rc << endl;
					exit(-1);
				}
			}
		}
	}
	 //  waiting for the other threads to complete
	 for(unsigned int i = 0; i < ptable.size(); i++){
		pthread_join(tid[i],NULL);
	 }
	
	//Cleaning up and exiting
	pthread_attr_destroy(&attr);
	pthread_mutex_destroy(&mylock);
	pthread_cond_destroy(&clear);
	pthread_exit(NULL);
	
	
	return 0;
}

void *Capulet(void *arg){
	
	string gang_name, person_name;
	int ar_time, p_time;
	struct personinfo *argptr;
	argptr = (struct personinfo *) arg;
	
	gang_name = argptr->gang;
	person_name = argptr->name;
	ar_time = argptr->atime;
	p_time = argptr->ptime;
	
	
	cout << gang_name << " " << person_name << " has arrived at time " << ar_time << endl;
	pthread_mutex_lock(&mylock);
	while(moncount > 0){
		pthread_cond_wait(&clear, &mylock);
	}
	cout << gang_name << " " << person_name << " has entered the plaza" << endl;
	capcount++;
	pthread_cond_signal(&clear);
	pthread_mutex_unlock(&mylock);
	sleep(p_time);
	
	pthread_mutex_lock(&mylock);
	cout << gang_name << " " << person_name << " leaves the plaza" << endl;
	capcount--;
	if(capcount == 0){
		pthread_cond_broadcast(&clear);
	}
	pthread_mutex_unlock(&mylock);
	

	pthread_exit((void*) 0);
}//Capulet thread

void *Montague(void *arg){
	
	string gang_name, person_name;
	int ar_time, p_time;
	struct personinfo *argptr;
	argptr = (struct personinfo *) arg;
	
	gang_name = argptr->gang;
	person_name = argptr->name;
	ar_time = argptr->atime;
	p_time = argptr->ptime;
	

	cout << gang_name << " " << person_name << " has arrived at time " << ar_time << endl;
	pthread_mutex_lock(&mylock);
	while(capcount > 0){
		pthread_cond_wait(&clear, &mylock);
	}
	cout << gang_name << " " << person_name << " has entered the plaza" << endl;
	moncount++;
	pthread_cond_signal(&clear);
	pthread_mutex_unlock(&mylock);
	sleep(p_time);
	
	pthread_mutex_lock(&mylock);
	cout << gang_name << " " << person_name << " leaves the plaza" << endl;
	moncount--;
	if(moncount == 0){
		pthread_cond_broadcast(&clear);
	}
	pthread_mutex_unlock(&mylock);
	
	pthread_exit((void*) 0);
}//Montague thread
