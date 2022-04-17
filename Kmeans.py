import sys 
import math
# confirmed with naser its ok!

def checktype(str):
    try:
        bol=(int(str)==float(str))
        return not bol
    except:
        print("An Error Has Occurred")
        sys.exit(1)

arglist=sys.argv
if len(sys.argv)==5:
    if (checktype(arglist[1]) or int(arglist[1])<2):
        print("Invalid Input!")
        sys.exit(1)
    if (checktype(arglist[2]) or int(arglist[2])<1):
        print("Invalid Input!")
        sys.exit(1)
    K=int(arglist[1])
    Max_iter=int(arglist[2])
    In_filename=arglist[3]
    Out_filename=arglist[4]
elif len(sys.argv)==4:
    if (checktype(arglist[1]) or int(arglist[1])<2):
        print("Invalid Input!")
        sys.exit(1)
    K=int(arglist[1])
    In_filename=arglist[2]
    Out_filename=arglist[3]
EPSILON=0.001
if 'Max_iter' not in locals():
    Max_iter=200

f=open(In_filename,'r')
point_list=f.read().split('\n')[:-1]
point_list=[tuple([float(i) for i in point.split(',')]) for point in point_list]
if len(point_list)<K:
    print("Invalid Input!")
    sys.exit(1)
base_centorid_list=point_list[:K]
centroid_to_point_dict={centroid :[] for centroid in base_centorid_list}
f.close()

#vector operations
def vector_sub(point1,point2):
    res_vector={}
    point1=list(point1)
    point2=list(point2)
    for i in range(len(point1)):
        res_vector[i]=0
    for i in range(len(point1)):
        res_vector[i]+=(point1[i]-point2[i])
    return (res_vector[i] for i in range(len(res_vector)))

def vector_dot_product(vector1):
    res=0
    vector1=list(vector1)
    for i in range(len(vector1)):
        res+=pow(vector1[i],2)
    return res

def mean_of_vector_list(vector_list):
    sum_vector={}
    for i in range(len(vector_list[0])):
        sum_vector[i]=0
    count=len(vector_list)
    if count==0:
        print("An Error Has Occurred")
        sys.exit(1)
    for vector in vector_list:
        for i in range(len(vector)):
            sum_vector[i]+=vector[i]
    return tuple(sum_vector[i]/count for i in range(len(sum_vector)))

def calc_euclid_norm(vector1,vector2):
    res=0
    for i in range(len(vector1)):
        res+=pow((vector2[i]-vector1[i]),2)
    return math.sqrt(res)

#cluster operations
def find_argmin_cluster_to_point(centroid_list,point):
    min=sys.maxsize
    res=None
    for centroid in centroid_list:
        temp=vector_dot_product(vector_sub(point,centroid))
        if temp<min:
            min=temp
            res=centroid
    return res

def update_centroids(centroid__to_point_dict,base_centorid_list):
    new_average_list=[]
    for i in range(len(base_centorid_list)):
        base_centorid_list[i]=list(centroid__to_point_dict.keys())[i]
    for key in list(centroid__to_point_dict.keys()):
        value=centroid__to_point_dict[key]
        average=mean_of_vector_list(value)
        new_average_list.append(average)
    centroid__to_point_dict.clear()
    for average in new_average_list:
        centroid__to_point_dict[average]=[]

def check_euclid_to_centroids(base_centroid,new_centroid_list,EPSILON): 
    for i in range(len(base_centorid_list)):
        #print(calc_euclid_norm(base_centroid[i],new_centroid_list[i]))
        if calc_euclid_norm(base_centroid[i],new_centroid_list[i])<EPSILON:
            continue
        else:
            return False
    return True
 
#run it all, first call is to get initial centroids 
def main(centroid_to_point_dict,point_list):
    for point in point_list:
        cluster=find_argmin_cluster_to_point(centroid_to_point_dict.keys(),point)
        centroid_to_point_dict[cluster].append(point)
    update_centroids(centroid_to_point_dict,base_centorid_list)

#print(base_centorid_list)
#print('**************************************************')
main(centroid_to_point_dict,point_list)
#print(centroid_to_point_dict)
i=1

while i<Max_iter and not check_euclid_to_centroids(base_centorid_list,list(centroid_to_point_dict.keys()),EPSILON):
    main(centroid_to_point_dict,point_list) 
    i+=1

#print(centroid_to_point_dict.keys())
#save to output file
with open(str(Out_filename),'w') as f:
    formatted_list = [tuple('%.4f'%cord for cord in key) for key in list(centroid_to_point_dict.keys())]
    for point in formatted_list:
        line=",".join(point)
        f.write(line+'\n')
    f.close()



