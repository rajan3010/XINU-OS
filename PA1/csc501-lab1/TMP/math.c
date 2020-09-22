#include<math.h>

double pow(double x, int y){
    double pow=1.0;
    int i=1;
    while(i<=y){
        pow=pow*x;
        i=i+1;
    }
    return pow;
}

double log(double x){
    int precision=20; //this high precision was chosen, to accurately calculate ln function for very small values of dummy=rand()/RAND_MAX which occurs for minimal values of rand()
    int i;
    double y;
    double sum;

    if(x<1){
    sum=0;
    i=1;
    y=1-x;
    while(i<=precision){
        sum=sum+(pow(y, i))/i;
        i=i+1;
    }
    sum=sum*-1;
    }
    else{
        i=1;
        y=x-1;
        sum=0;

        while(i<=precision){
            sum=sum+(pow(-1, i+1)*(pow(y, i)/i));
            i=i+1;
        }
    }

    return sum;
}

double expdev(double lambda) {
    double dummy;
    long random;
    do{
        random=rand();
        dummy= (double)random/RAND_MAX;
    }
    while (dummy == 0.0);
    //kprintf("\n\n The dummy number is: %d", (int)(dummy*10));
    return -log(dummy) / lambda;
}