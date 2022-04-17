#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <limits.h>
#include <math.h>
#include <sys/types.h>

struct cluster
{
    double* currCentroid;
    double* prevCentroid;
};

ssize_t getline(char **, size_t*, FILE *);
void printArray(double*, int);
double* subtract_vectors(double* res_vector,double* vector1,double* vector2, int d);
double dot_product(double* vector1,int d);
int find_cluster_to_point(double* res_vector,struct cluster** cluster_list,double* point,int K,int d);
void find_cluster_to_all_points(double* res_vector,struct cluster** cluster_list,double **points_list, int* point_cluster_array,int row_counter,int K,int d);
int update_centroid(double* temp,struct cluster** cluster_list,double **points_list, int* point_cluster_array,int row_counter,int K,int d);
int check_epsilon_distance(double* res_vector,struct cluster** cluster_list, int K,int d);


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

/*vector operations*/
double* subtract_vectors(double* res_vector,double* vector1,double* vector2,int d){
    int i;
    for (i=0;i<d;i++){
            res_vector[i]=vector1[i]-vector2[i];
        }
    return res_vector;
}
double dot_product(double* vector1,int d){
        double res=0;
        int i;
        for (i=0;i<d;i++){
              res+=vector1[i]*vector1[i];
          }
        return res;
    }
/*find the min cluster to a single point and return it*/
int find_cluster_to_point(double* res_vector,struct cluster** cluster_list,double* point,int K,int d){
    int min_clust;
    double min_value=__FLT_MAX__;
    double temp_value;
    int i;
    for (i=0;i<K;i++){
        temp_value=dot_product(subtract_vectors(res_vector,point,cluster_list[i]->currCentroid,d),d);
        if (temp_value<min_value){
            min_value=temp_value;
            min_clust=i;
        }
    }
    return min_clust;
}
/*find to all points and assign to point_cluster_array the min clusters so that point_cluster_array[i] is the cluster of points_list[i]*/
void find_cluster_to_all_points(double* res_vector,struct cluster** cluster_list,double **points_list, int* point_cluster_array,int row_counter,int K,int d){
    int i;
    for (i=0;i<row_counter;i++){
        point_cluster_array[i]=find_cluster_to_point(res_vector,cluster_list,points_list[i],K,d);

    }
}

/*update centroid - run on point_cluster_array, sum corresponding points and average out*/
int update_centroid(double* temp,struct cluster** cluster_list,double **points_list, int* point_cluster_array,int row_counter,int K,int d){
    int i;
    int count;
    int j;
    int k;
  
    for (i = 0; i< K ; i++){
        count=0;
        for(j =0; j< row_counter ; j++){
            if(i==point_cluster_array[j]){ 
                count+=1;
                for (k=0;k<d;k++){
                     cluster_list[i]->prevCentroid[k]+=points_list[j][k];
                }                    
            }
        }
        printf("%d\n",i);
        if (count==0){
            printf("An Error Has Occurred");
            exit(1);
        }
        for (k=0;k<d;k++){
             cluster_list[i]->prevCentroid[k]= cluster_list[i]->prevCentroid[k]/count;
        }
        for (j = 0; j < d; j ++) {
            temp[j]=cluster_list[i]->prevCentroid[j];
        }
        for (j = 0; j < d; j ++) {
            cluster_list[i]->prevCentroid[j]=cluster_list[i]->currCentroid[j];
            cluster_list[i]->currCentroid[j]=temp[j];
    }}
    return 0;
      
    }

/*check if the delta of centroids is less than epsilon*/
int check_epsilon_distance(double* res_vector,struct cluster** cluster_list,int K,int d){
    int i;
    for(i=0;i<K;i++){
        double res;
        res=sqrt(dot_product(subtract_vectors(res_vector,cluster_list[i]->currCentroid,cluster_list[i]->prevCentroid,d),d));
        if (res>0.001){
            return 1;
        }

    }
    return 0;
}

int main(int argc, char** argv)
{
    FILE * fp;
    FILE * of;
    char * line = NULL;
    double **points_list;
    int d=1;
    ssize_t read;
    size_t len = 0;
    
    int K;
    int max_iter;
    int input_file;
    int output_file;
    int i;
    char *token;
    double* roni;
    struct cluster** clusters;
    int row_counter;
    int j;
    int* point_cluster_array;
    int success_update;
    double single_float;
    int iteration;
    double* temp;

    /*build arrays*/
    double* res_vector;

    temp=calloc(d,sizeof(double));
    if(temp==NULL){
        printf("An Error Has Occured");
        exit(1);
    };
    res_vector=calloc(d,sizeof(double));
    if(res_vector==NULL){
        printf("An Error Has Occured");
        exit(1);
    };

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
    } 
   
    /*find n,d in order to build matrix*/
    fp = fopen(argv[input_file], "r");
    if (fp == NULL)
        exit(1);
    
    row_counter=0;
    while ((read = getline(&line, &len, fp)) != -1) {
        row_counter++;
        if (d==1){
            for (i=0; line[i]; line[i]==',' ? d++,*line++ : *line++);
        }
    }
    
    if (row_counter<=K){
        printf("Invalid Input!");
        return 1;
    }

    /*build matrix*/
    points_list=calloc(row_counter,sizeof(void*));
    if(points_list==NULL){
        printf("An Error Has Occurred");
        return 1;
    };
    for (i=0;i<row_counter;i++){
        points_list[i]=calloc(d,sizeof(double));
        if(points_list[i]==NULL){
        printf("An Error Has Occurred");
        return 1;
    };
    }
    
    
    /*load values to matrix*/
    rewind(fp);
    i=0;
    while ((read = getline(&line, &len, fp)) != -1) {
        int j=0;
        token=strtok(line,",");
        while (token!=NULL){
            roni=points_list[i];
            roni[j]=atof(token);
            token=strtok(NULL,",");
            j++;
        }
        i++;
        }
    
    /*build k centroid array*/
    clusters = calloc(K,sizeof(struct cluster*));
    if(clusters==NULL){
        printf("An Error Has Occurred");
        return 1;
    }
    
    for(i=0 ; i<K ; i++ ){
        clusters[i] = malloc(sizeof(struct cluster));
        if(clusters[i]==NULL){
        printf("An Error Has Occurred");
        return 1;
    }
        clusters[i]->currCentroid = calloc(d, sizeof(double));
        clusters[i]->prevCentroid = calloc(d, sizeof(double));
        if(clusters[i]->currCentroid==NULL){
            printf("An Error Has Occurred");
            return 1;
        }
        for(j=0;j<d;j++) {
            clusters[i]->currCentroid[j] = points_list[i][j];
            clusters[i]->prevCentroid[j] = points_list[i][j];
        }

    }

    /*build array that connects point to cluster by index*/
    
    point_cluster_array=calloc(row_counter,sizeof(int));
    if(point_cluster_array==NULL){
        printf("An Error Has Occured");
        exit(1);
    };

    
    iteration=1;
    find_cluster_to_all_points(res_vector,clusters,points_list,point_cluster_array, row_counter,K,d);
    
    success_update=update_centroid(temp,clusters,points_list,point_cluster_array,row_counter,K,d);
    if (success_update==1){
        printf("An Error Has Occurred");
        exit(1);
    }
    while ((iteration<max_iter) && (check_epsilon_distance(res_vector,clusters,K,d)==1)){
            find_cluster_to_all_points(res_vector,clusters,points_list,point_cluster_array, row_counter,K,d);
            success_update=update_centroid(temp,clusters,points_list,point_cluster_array,row_counter,K,d);
            if (success_update==1){
                printf("An Error Has Occurred");
                exit(1);
    }
            iteration +=1;
    }

    of  = fopen(argv[output_file], "w");
    if (of==NULL){
        printf("An Error Has Occurred");
        exit(1);
    }
    
    for (i=0;i<K;i++){
        int j;
        for(j=0;j<d-1;j++){
            single_float=clusters[i]->currCentroid[j];
            /*printf("%.4f",single_float);*/
            fprintf(of,"%.4f",single_float);     
            fputs(",",of);
            /*printf(",");*/
        }
        single_float=clusters[i]->currCentroid[d-1];
        /*printf("%.4f",single_float);*/
        fprintf(of,"%.4f",single_float);
        fputs("\n",of);  
        /*printf("\n");  */
    }
    fclose(of);
    free(clusters);
    free(point_cluster_array);
    free(points_list);
    return 0;
}