#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <math.h>
#include <sys/types.h>
/*functions:*/
void printArray(double*, int);
void printArrayInt(int*, int);

int check_epsilon_distance(double **curCent,double **prevCent,int K,int d);
double dot_product(double* vector1,int d);
double dot_product_sub(double* vector1,double* vector2,int d);

/*check if the delta of centroids is less than epsilon*/
int check_epsilon_distance(double **curCent,double **prevCent,int K,int d){
    int i;
    for(i=0;i<K;i++){
        double res;
        res=dot_product_sub(curCent[i],prevCent[i],d);
        if (res>0.001){
            return 1;
        }

    }
    return 0;
}


double dot_product(double* vector1,int d){
        double res=0;
        int i;
        for (i=0;i<d;i++){
              res+=pow(vector1[i],2);
          }
        return sqrt(res);
    }

double dot_product_sub(double* vector1,double* vector2,int d){
        double res=0;
        int i;
        for (i=0;i<d;i++){
              res+=pow(vector1[i]-vector2[i],2);
          }
        return sqrt(res);
    }


void printArray(double* array,int d){
    int j;
    for(j = 0; j < d; j++) {
        if(j<d-1){
            printf("%f,", array[j]);
        }
        else{
            printf("%f", array[j]);
        }
    }
}

void printArrayInt(int* array,int d){
    int j;
    for(j = 0; j < d; j++) {
        if(j<d-1){
            printf("%d,", array[j]);
        }
        else{
            printf("%d", array[j]);
        }
    }
}


int main(int argc, char** argv){
    FILE * fp;
    FILE * of;
    int k;
    int count;
    double **points_list;
    int d=1;
    int K;
    int max_iter;
    int input_file;
    int output_file;
    int i;
    int row_counter;
    int j;
    long len;
    char *array_input_char;
    char * token1;
    double **curClusters;
    double **prevClusters;
    int * point_cluster_array;
    double single_float;
    int iteration;
    int min_clust;
    double min_value=__FLT_MAX__;
    double temp_value;
    int strlen_1;
    
    fp = NULL;
    of = NULL;




/*check input*/
    if (argc==5){
        K=atoi(argv[1]);
        if ((K!=atof(argv[1]))|( K<2)){
            printf("Invalid Input!");
            return 1;
        }
        max_iter=atoi(argv[2]);
        if ((max_iter!=atof(argv[2])) | (max_iter<1)){
            printf("Invalid Input!");
            return 1;
        }
        input_file=3;
        output_file=4;
    }
    else if (argc==4){
        K=atoi(argv[1]);
        if ((K!=atof(argv[1])) | (K<2)){
            printf("Invalid Input!");
            return 1;
        }
        input_file=2;
        output_file=3;
        max_iter=200;
    }
    else{
        printf("Invalid Input!");
        return 1;
    }

/*recieve input*/
    fp = fopen(argv[input_file], "r");
    if (fp == NULL) {
        printf("An Error Has Occurred");
        return 1;
    }
    
    fseek(fp, 0, SEEK_END);
    len = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    array_input_char = malloc(sizeof(char) * (len+ 1));
    if (array_input_char == NULL) {
        printf("An Error Has Occurred");
        return 1;
    }
    array_input_char[len] = '\0';
    
    fread(array_input_char, sizeof(char), len, fp);
    fclose(fp);
    
    /* find the value of d */
    
    for (token1 = array_input_char; *token1 != '\n'; ++token1) 
    {
        if (*token1 == ',')
        {
             ++d;
        }
    }
  
    /* find row_counter */
    row_counter = 0;
    for ( ; *token1 != '\0'; ++token1) {
        if (*token1 == '\n') { 
            ++row_counter; 
            *token1 = '\0';
            }
    }
    
   
    /* insert values from strings to doubles tp the final matrix*/   
    points_list = calloc(row_counter, sizeof(double*));
    if (points_list == NULL) {
        printf("An Error Has Occurred");
        return 1;
    }

    for (i = 0 ; i < row_counter ; i++) {
        points_list[i] = calloc(d, sizeof(double));
        if (points_list[i] == NULL) {
        printf("An Error Has Occurred");
        return 1;
    }
    }   

    for (i = 0 ; i < row_counter ; i++) {
        for ( token1 = array_input_char; *token1 != '\0'; ++token1) 
        { 
            if (*token1 == ',') 
                { 
                *token1 = '\0';
                }
        }
        for (j = 0; j < d; j ++) {
            points_list[i][j] = atof(array_input_char);
            strlen_1= strlen(array_input_char);
            array_input_char = array_input_char + strlen_1 + 1;
        }
    }    
    /*bulid k curcent and prevcent array*/
    curClusters = malloc(K*sizeof(double*));
    prevClusters = malloc(K*sizeof(double*));
    if((prevClusters==NULL)||(curClusters==NULL)){
        printf("An Error Has Occurred");
        return 1;
    }

    for(i=0 ; i<K ; i++ ){
        curClusters[i] = malloc(d*sizeof(double));
        prevClusters[i] = malloc(d*sizeof(double));
        if((curClusters[i]==NULL)||(prevClusters[i]==NULL)){
            printf("An Error Has Occurred");
            return 1;
        }
        for(j=0;j<d;j++) {
            curClusters[i][j] = points_list[i][j];
            prevClusters[i][j] = points_list[i][j];
        }
    }
    
     /*build array that connects point to cluster by index*/
    point_cluster_array=malloc(row_counter*sizeof(int));
    if(point_cluster_array==NULL){
        printf("An Error Has Occured");
        exit(1);
    };

    /*find to each point her centroid*/
    for(i =0; i< row_counter;i++){
        min_value=__FLT_MAX__;
        for(j=0;j<K;j++){
            temp_value = dot_product_sub(points_list[i], curClusters[j],d);


             if (temp_value<min_value){
                min_value=temp_value;
                min_clust=j;
            }

        }
        point_cluster_array[i]=min_clust;
    }
   /* printArrayInt(point_cluster_array,row_counter);*/
/*finds new centroid*/
    for(i=0 ; i<K ; i++){
        count =0;
        for(j=0 ;j<d ;j++){
            curClusters[i][j]=0.0;
        }
        for(j =0; j< row_counter ; j++){
            if(i==point_cluster_array[j]){
                count+=1;
                for (k=0;k<d;k++){
                    curClusters[i][k]+=points_list[j][k];
                }                    
            }
        }
        if (count==0){
            printf("An Error Has Occurred");
            exit(1);
        }
        for (k=0;k<d;k++){
            curClusters[i][k]=curClusters[i][k] / count;
        }
    }


    iteration=1;

    while ((iteration<max_iter) && (check_epsilon_distance(curClusters,prevClusters,K,d)==1)){
        iteration+=1;
        /*find cluster to all points*/
        for(i =0; i< row_counter;i++){
            min_value=__FLT_MAX__;
            for(j=0;j<K;j++){
                temp_value = dot_product_sub(points_list[i], curClusters[j],d);
                if (temp_value<min_value){
                    min_value=temp_value;
                    min_clust=j;
                }
            }
            point_cluster_array[i]=min_clust;
        }
        /*copy cur to prev before the change*/
        for(i=0 ; i<K ; i++){
            for(j=0;j<d;j++){
                prevClusters[i][j]=curClusters[i][j];
                curClusters[i][j]=0.0;
            }
        }

        /*update centroid*/
        for(i=0 ; i<K ; i++){
            count =0;
            for(j =0; j< row_counter ; j++){
                if(i==point_cluster_array[j]){
                    count+=1;
                    for (k=0;k<d;k++){
                        curClusters[i][k]+=points_list[j][k];
                    }                    
                }
            }
            if (count==0){
                printf("An Error Has Occurred");
                exit(1);
            }
            for (k=0;k<d;k++){
                curClusters[i][k]=curClusters[i][k]/count;
            }
        }
    }
/*write back to the file*/

    of  = fopen(argv[output_file], "w");
    if (of==NULL){
        printf("An Error Has Occurred");
        exit(1);
    }
    
    for (i=0;i<K;i++){
        int j;
        for(j=0;j<d-1;j++){
            single_float=curClusters[i][j];
            fprintf(of,"%.4f",single_float);     
            fputs(",",of);
        }
        single_float=curClusters[i][d-1];
        
        fprintf(of,"%.4f",single_float);
        fputs("\n",of);  
        
    }
    fclose(of);
    free(curClusters);
    free(prevClusters);
    free(point_cluster_array);
    free(points_list);
    
    

    return 0;

}
