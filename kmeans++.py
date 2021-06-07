import  mykmeanssp
import sys
import pandas as pd
import numpy as np
import timeit
global vector_list
global clusters

clusters = []
def main(max_iter=200):
    start = timeit.default_timer()
    num = len(sys.argv)
    assert( num == 4 or num == 5) #Check if 5 is CORRECT
    if (num == 5):
        max_iter = int (sys.argv[2])
        assert (max_iter >= 0)
        file1_name = sys.argv[3]
        file2_name = sys.argv[4]
    else:
        file1_name = sys.argv[2]
        file2_name = sys.argv[3]
    K = int (sys.argv[1])
    assert (K >= 0)
    vector_list = readfile(file1_name,file2_name)
    vector_list.index = vector_list.index.astype('int64')
    vector_num = vector_list.shape[0]
    vector_len = vector_list.shape[1]
    centroid_for_C, centroids_index = init_centroids(K, vector_list.values,vector_list.index,vector_num,vector_len)
    vector_list_to_C = vector_list.values.tolist()
    final_centroids = mykmeanssp.fit(centroid_for_C,vector_list_to_C,max_iter,K)
    converted_indexes = [str(elm) for elm in centroids_index]
    joined_indexes = ",".join(converted_indexes)
    print(joined_indexes)
    for centroid in final_centroids:
        for i in range(len(centroid)):
            centroid[i] = np.round(centroid[i],4)
        print(*centroid, sep = ", ")  
            
def readfile(file1_name,file2_name):
    file1 = pd.read_csv(file1_name,index_col=0,header=None).sort_index()
    file2 = pd.read_csv(file2_name,index_col=0,header=None).sort_index()
    vector_list = file1.merge(file2, left_index=True, right_index=True)
    return vector_list

def init_centroids(k,vector_list,vector_list_ind,vector_num,vector_len):
    np.random.seed(0)
    assert k<vector_num
    dist = [0 for i in range(vector_num)]
    centroid_for_C = [0 for i in range(k)]
    centroids_index = [0 for i in range(k)]
    #first centroid
    rand_index = np.random.randint(0,vector_num+1)
    centroid_for_C[0]= vector_list[rand_index]
    centroids_index[0] = vector_list_ind[rand_index]
    z=1
    while z<k:
        #calc dist
        for i in range(vector_num):
            if z==1:
                 dist[i] = distance(vector_list[i], centroid_for_C[0])
            else:
                dist[i] = min_dist_centroid(vector_list,i,centroid_for_C,z,dist)
        #calc prob
        sum = np.sum(dist)
        prob = dist/sum
        # find new centroid
        chosen_ind = np.random.choice(vector_num,p=prob)
        centroid_for_C[z]= vector_list[int(chosen_ind)]
        centroids_index[z] = vector_list_ind[int(chosen_ind)]
        z+=1
    #convert to python lists
    for i in range(k):
        centroid_for_C[i] = centroid_for_C[i].tolist()
    return centroid_for_C, centroids_index


def distance(v1,v2):
    return  np.sum((v1-v2)**2)

def min_dist_centroid(vector_list,i,centroid_for_C,z,dist):
    return min(distance(vector_list[i], centroid_for_C[z-1]), dist[i])



if(__name__ == "__main__"):
    main()
