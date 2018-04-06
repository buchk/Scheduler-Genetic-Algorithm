#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include "example.h"
#include <iostream>
#include <fstream>


using namespace std;
// file.c
/////////////  Pure C Code ////////////////


#define POPULATION_SIZE 1000
#define true 1
#define false 0
int popn, maxgen, pc, pm;
typedef enum days
{
	MWF,
	TH,
	LAST_DAY
} days;


char* dayNames [LAST_DAY] = { "MWF", "TH" };

TimePeriod timePeriods[NUM_TIME_PERIODS] =
{
	{0,MWF,8},{1,MWF,9},{2,MWF,10},{3,MWF,11},{4,MWF,12},{5,MWF,1},{6,MWF,2},{7,MWF,3},{8,TH,8},{9,TH,930},{10,TH,11},{11,TH,1230},{12,TH,2},{13,TH,330}
};

Room rooms[NUM_ROOMS] = {
	{0,"BL134",30,true},
	{1,"BL138",50,true},
	{2,"KR224",40,false},
	{3,"KR124",20,false},
	{4,"KR206",30,true},
	{5,"Biddle123",35,false},
	{6,"Biddle125",40,false},
	{7,"Biddle205",45,false},
	{8,"ES100",100,true} };

Professors professors[NUM_PROFS] =
{
	{0,"Hagerich"},
	{1,"Bilitski"},
	{2,"Smigla"},
	{3,"Im"},
	{4,"Frederick"},
	{5,"Thomson"},
	{6,"Darling"},
	{7,"Hinderliter"},
	{8,"Ferencek"}
};

// the definition of a course.  Excludes room and time.  Just a helper to fill in default values
Course coursesStub[NUM_COURSES] =
{
	{0,"cs015",0,20,false},
	{1,"cs456",1,20,true},
	{2,"cs456",1,20,true},
	{3,"cs456",1,20,true},
	{4,"cs1783",1,15,true},
	{5,"cs455",0,20,true},
	{6,"cs015",2,35,true},
	{7,"cs015",3,35,false},
	{8,"cs015",3,35,false},
	{9,"cs015",4,35,false},
	{10,"math001",8,40,false},
	{11,"math001",8,50,false},
	{12,"math001",8,60,false},
	{13,"math002",5,40,false},
	{14,"math002",5,50,false},
	{15,"math002",5,60,false},
	{16,"soc100",6,45,true},
	{17,"soc100",6,40,true},
	{18,"soc100",6,35,true},
	{19,"cs047",1,15,true},
	{20,"cs047",1,15,true},
	{21,"psy200",7,30,false},
	{22,"psy200",7,35,false},
	{23,"psy200",7,30,false},
	{24,"cs045",0,20,true},
	{25,"cs045",0,20,true},
	{26,"cs015",2,20,true}
};


Schedule population[POPULATION_SIZE];  // an array schedules
int MaxFitness = -99999999;
Schedule bestschedule;



char* getProfessorName ( int pid )
{
	return professors[pid].professorName;
}

char* getRoomName (int rid )
{
	return rooms[rid].roomName;
}

int getRoomSize ( int rid)
{
	return rooms[rid].roomSize;
}



void printCourse(Course* cptr)
{
	char * profName;
	char * rmName;
	int roomSize;
	
	profName = getProfessorName(cptr -> professorID);
	rmName = getRoomName(cptr->roomID);
	roomSize = getRoomSize(cptr ->roomID);
	
	printf("%i\t%.10s\t%10s\t%5i\t%i\t            %10s\t%i\t     %i\t\t  %5s\t   %5i\n",
		   cptr->crn,
		   cptr->coursename,
		   profName,
		   cptr->courseSize,
		   cptr->multimedia,
		   rmName,
		   roomSize,
		   rooms[cptr->roomID].multimedia,
		   dayNames[timePeriods[cptr->timePeriodID].days],
		   timePeriods[cptr->timePeriodID].startTime

		   );
		   
}


void printPopulation ( void )
{
	int i;

	for ( i = 0; i < POPULATION_SIZE; ++i )
	{
		printf ("\n\nCRN\tCourseName\tProf\tCrsSize\tNeedsMultimedia \tRmName\tRmSize\tRmHasMM\t    Days\tTime\n");
		int j;
		for ( j = 0; j < NUM_COURSES; ++j)
		{
			printCourse( &population[i].schedule[j] );
		}
		cout << "Fitness: " << population[i].fitness << endl;
	}
	
	
}

void initializePopulation ( void )
{
	srand(time(NULL));
	
	// start by just copying the definition data in
	int i;
	for ( i = 0; i < POPULATION_SIZE; ++i )
	{
		memcpy(&population[i].schedule,coursesStub,sizeof(coursesStub));
	
		// put in a random room and time
		int j;
		for (j=0; j<NUM_COURSES; ++j)
		{
			population[i].schedule[j].roomID =  rand() % NUM_ROOMS;
			population[i].schedule[j].timePeriodID = rand() % NUM_TIME_PERIODS;
		}
		
		
	}
	
}

void getglobalvars(void)
{
	float pctemp, pmtemp;
	
	cout << "Enter the population size (N) : " << endl;
	cin >> popn;
	cout << endl << "Enter the max number of generations: " << endl;
	cin >> maxgen;
	cout << endl << "Enter the probability of crossover (Pc) : " << endl;
	cin >> pctemp;
	cout << endl << "Enter the probablity of mutation (Pm) : " << endl;
	cin >> pmtemp;
	
	pc = ((int)(pctemp * 100.0));
	pm = ((int)(pmtemp * 100.0));
	
}

struct maxpasser
{
	int maxfitness;
	int minfitness;
	int avgfitness;
	
	
	
	
	
};


maxpasser EvaluateFitness(void)
{
	int avgfitness = 0;
	int tempfitness = 0;
	int currentprof = 0;
	int currentroomsize = 0;
	struct maxpasser temppasser;
	temppasser.maxfitness = -9999;
	temppasser.minfitness = 9999;
	
	for (int i = 0; i < popn; i++) // generation loop
	{
		tempfitness = 0;
		
		for (int j = 0; j < 27; j++) // schedule loop
		{
			currentprof = population[i].schedule[j].professorID;
			
			//do evaluations on roomsize and multimedia
			
			currentroomsize = rooms[population[i].schedule[j].roomID].roomSize;
			
			
			
			if (currentroomsize >= population[i].schedule[j].courseSize) //see if room is big enough
			{
				tempfitness = tempfitness + 20;
			}
			else
			{
				tempfitness = tempfitness - 70;
			}
			
			if ((population[i].schedule[j].multimedia == true) && (rooms[population[i].schedule[j].roomID].multimedia == true)) // see if it needed multimedia and got it
			{
				tempfitness = tempfitness + 20;
			}
			if ((population[i].schedule[j].multimedia == true) && (rooms[population[i].schedule[j].roomID].multimedia == false))	//see if it needed it and didn't get it
			{
				tempfitness = tempfitness - 50;
				
			}	
				
				for (int l = (j + 1); l < 27; l++)  //comparison schedule loop
				{
					
					if ((currentprof == population[i].schedule[l].professorID) && (population[i].schedule[j].timePeriodID == population[i].schedule[l].timePeriodID))
					{
						tempfitness = tempfitness - 300;
					}
					
					if ((population[i].schedule[j].timePeriodID == population[i].schedule[l].timePeriodID) && (population[i].schedule[j].roomID == population[i].schedule[l].roomID)) 
					{
						tempfitness = tempfitness - 300;
					}
					
				}
				
			
			
			
			
			
			
		}
		
		avgfitness = avgfitness + tempfitness;
		population[i].fitness = tempfitness;
		
		if (population[i].fitness > MaxFitness) //copies the fitness/accompanying schedule of best global fitnesss so far
		{
			MaxFitness = population[i].fitness;
			
			memcpy(&bestschedule, population[i].schedule, sizeof(population[i].schedule));
			
			
		}
		
		if (population[i].fitness > temppasser.maxfitness) //copies this generation's max fitness for return
		{
			
			temppasser.maxfitness = population[i].fitness;
		}
		
		if (population[i].fitness < temppasser.minfitness) //copies this generation's min fitness for return
		{
			
			temppasser.minfitness = population[i].fitness;
		}
	}
	
	temppasser.avgfitness = avgfitness / popn;
	
	return temppasser;
	
}

Schedule pickparent(void)
{
	Schedule result1, result2, result;
	int randomscheduleindex, randomscheduleindex2;
	
	randomscheduleindex = rand() % popn;
	
	randomscheduleindex2 = rand() % popn;
	
	if (population[randomscheduleindex].fitness > population[randomscheduleindex2].fitness)
	{
		result = population[randomscheduleindex];
	}
	else
	{
		result = population[randomscheduleindex2];
	}
	 

		
	return result;
	
	
}

void createnextgen(void)
{
	Schedule parent1, tempparent1;
	Schedule parent2, tempparent2;
	int crosscheck, crossindex;
	int mutationcheck, mutationspot1, mutationspot2;
	Schedule temppopulation[popn];
	for (int j = 0; j < popn; )
	{
		parent1 = pickparent();
		parent2 = pickparent();  // i checked, definitely have schedules
		
		while (parent1.fitness == parent2.fitness)
		{
			parent2 = pickparent();
			//cout << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl << endl << endl << endl << endl << "%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%" << endl;
			
		}
		crosscheck = rand() % 100;
		if (crosscheck <= pc)
		{
			crossindex = rand() % 27;
			for (int i = crossindex; crossindex < 27; crossindex++)
			{	
				tempparent1.schedule[i].timePeriodID = parent1.schedule[i].timePeriodID; //this block crosses over
				tempparent1.schedule[i].roomID = parent1.schedule[i].roomID;
				tempparent2.schedule[i].timePeriodID = parent2.schedule[i].timePeriodID;
				tempparent2.schedule[i].roomID = parent2.schedule[i].roomID;
				
				
				
				parent1.schedule[i].timePeriodID = tempparent2.schedule[i].timePeriodID; // this block copies temp variables to variables to be put in new generation
				parent1.schedule[i].roomID = tempparent2.schedule[i].roomID;
				parent2.schedule[i].timePeriodID = tempparent1.schedule[i].timePeriodID;
				parent2.schedule[i].roomID = tempparent1.schedule[i].roomID;
				
			}
			
		}
		
		mutationcheck = rand() % 100;
		mutationspot1 = rand() % 27;
		mutationspot2 = rand() % 27;
		if (mutationcheck <= pm)
		{
			parent1.schedule[mutationspot1].timePeriodID = parent1.schedule[mutationspot1].timePeriodID + 1; //mutation
			if (parent1.schedule[mutationspot1].timePeriodID > 13)
			{
				parent1.schedule[mutationspot1].timePeriodID = 0;
			}
			
			parent1.schedule[mutationspot1].roomID = parent1.schedule[mutationspot1].roomID + 1;
			
			if (parent1.schedule[mutationspot1].roomID > 8)
			{
				parent1.schedule[mutationspot1].roomID = 0;
			}
			
			
			parent2.schedule[mutationspot2].timePeriodID = parent1.schedule[mutationspot2].timePeriodID + 1;
			if (parent2.schedule[mutationspot2].timePeriodID > 13)
			{
				parent2.schedule[mutationspot2].timePeriodID = 0;
			}
			
			parent2.schedule[mutationspot2].roomID = parent1.schedule[mutationspot2].roomID + 1;
			
			if (parent2.schedule[mutationspot2].roomID > 8)
			{
				parent2.schedule[mutationspot2].roomID = 0;
			}
			
		}	
	
		memcpy(&temppopulation[j].schedule, parent1.schedule, sizeof(parent1.schedule));
		memcpy(&temppopulation[j+1].schedule, parent2.schedule, sizeof(parent2.schedule));
		//temppopulation[j].schedule = parent1.schedule;
		//temppopulation[j+1].schedule = parent2.schedule;
		j = j + 2;
	
	}
	
	memcpy(&population, temppopulation, sizeof(temppopulation));
	//population = temppopulation;
	
}




int main (int argc, char *argv[])
{
   struct maxpasser avgfitness;
   getglobalvars();  
   srand(time(NULL));
   initializePopulation();
   avgfitness = EvaluateFitness();
   printPopulation();
   
    ofstream myoutput;
	myoutput.open ("output.txt");
	
	
	
	
  
   for (int m = 0; m < maxgen; m++)
   {   
		createnextgen();
		avgfitness = EvaluateFitness();
		myoutput << m << ", " << avgfitness.avgfitness << ", " << avgfitness.maxfitness << ", " << avgfitness.minfitness << endl;
		if ((m % 100) == 0)
		{
			cout << "Gen: " << m << endl; 
			printPopulation();
		}
   }
   
   myoutput.close();
   
   
   //printPopulation();
   
   cout << endl << endl << endl << "Best Schedule: " << MaxFitness << endl;
   for (int q = 0; q < NUM_COURSES; ++q)
		{
			
			printCourse( &bestschedule.schedule[q] );
		}
   return 0;

}

