#define PY_SSIZE_T_CLEAN
#include <Python.h>

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>


double **clusters;
double **centroids; 
double **vector_list;
int* clustersindexes;
int vector_len;
int vector_num;
int clusters_num; 

void init_cendroids(int clusters_num) {
    int i,j,x;
    assert(clusters_num<vector_num);
    centroids = (double**) calloc(clusters_num, vector_len*sizeof(double));
    assert(centroids != NULL);
    for ( i=0; i<clusters_num; i++) {
        centroids[i] = (double*) calloc(vector_len,sizeof(double));
        assert(centroids[i] != NULL);
    }
    for ( x=0; x<clusters_num; x++) {
        for ( j=0; j<vector_len; j++) {
            centroids[x][j]=vector_list[x][j];
        }
    }
    clusters = (double**) calloc(clusters_num, sizeof(double*));
}

double distance(double *v1, double *v2) {
    double res;
    int length;
    int i;
    res = 0;
    length = vector_len;
    for ( i=0; i<length; i++) {
        res += (v1[i]-v2[i])*(v1[i]-v2[i]);
    }
    return res;
}

int min_dist_centroid(double *v){
    double minimum;
    int ind;
    double dist;
    int i;
    minimum = distance(v, centroids[0]);
    ind = 0;
    for ( i=0; i<clusters_num; i++){
        dist = distance(v, centroids[i]);
        if (dist < minimum){
            minimum = dist;
            ind = i;
        }
    }  
    return ind;
}

void vector_to_cluster(int clusters_num) {
    int* clusterssizes; /*for realloc*/
    int ind;
    int i;
    clustersindexes = (int *) calloc(clusters_num, sizeof(int));
    assert(clustersindexes != NULL);
    clusterssizes = (int *) calloc(clusters_num, sizeof(int));
    assert(clusterssizes != NULL);
    for ( i=0;i<clusters_num;i++) { /*initialize each cluster's size to 100*/
        clusterssizes[i] = 100;
    }
    for ( i=0; i<clusters_num; i++) {
        clusters[i] = (double*) calloc(100,sizeof(double));
        assert(clusters[i]!=NULL);
        }
    for ( i=0; i<vector_num; i++) {
        ind = min_dist_centroid(vector_list[i]);
        if (clustersindexes[ind]>((clusterssizes[ind])/2)) {/*Increase if necessary*/
            clusters[ind] = (double*) realloc(clusters[ind], 2*clusterssizes[ind]*sizeof(double));
            clusterssizes[ind]*=2;
        }
        clusters[ind][clustersindexes[ind]] = i;
        clustersindexes[ind]++; /*increase number of vectors in specified cluster*/

    }
}

double* cluster_to_centroid(int index){
    int i;
    int j;
    int vector_index;
    int num = clustersindexes[index]; /* number of vectors in given cluster */
    double * res = (double *)calloc(vector_len, sizeof(double));
    assert(res != NULL);
    for (i = 0; i < vector_len; i++) {
        for (j = 0; j < num; j++) {
            vector_index = clusters[index][j]; /* not actual vector but index in vector_list */
            res[i] += vector_list[vector_index][i]; /*relevant cluster*/
        }
    }
    for (i = 0; i < vector_len; i++) {
        res[i] = res[i]/(num); 
    }
    return res;
}

int areequal(double* arr1, double* arr2) {
    int length;
    int y;
    length = vector_len; 
    for ( y =0; y<length; y++) {
        if (arr1[y]!=arr2[y]) {
            return 0;
        }
    }
    return 1;
}

int update_centroids() {
    int changed = 0;
    int x,i,j;
    for ( i=0; i<clusters_num; i++) {
        double* newcentroid;
        newcentroid = (double*) calloc(vector_len,sizeof(double));
        assert(newcentroid != NULL);
        for ( j=0; j<vector_len; j++) {
            newcentroid[j] = cluster_to_centroid(i)[j];
        }
        if (areequal(centroids[i], newcentroid)==0) {
            changed++;
        }
        for ( x=0; x<vector_len; x++) {
            centroids[i][x] = newcentroid[x];
        }
    }
    return (changed!=0);
}


static double**  calccentroids(int max_iter) {
    int counter;
    int isequal;
    assert(clusters_num == 0 || clusters_num > 0);
    counter = 0;
    isequal = 1;
    clusters = (double**) calloc(clusters_num, sizeof(double*)); /* originally in init_centroids */
    while(counter<max_iter && isequal==1) {
        vector_to_cluster(clusters_num);
        isequal = update_centroids();
        counter++;
    }
    free(vector_list);
    free(clusters);
    free(clustersindexes);
    return centroids;
}

static PyObject* fit(PyObject *self, PyObject *args) 
{
    PyObject * centroids_list;
    PyObject * origin_vector_list;
    Py_ssize_t m;
    Py_ssize_t n;
    /*double** initial_centroids;  we used global centroids*/
    /*double** vector_list; we used global vector_list*/
    int max_iter;
    /*int centroid_size*/ /* K - replaced to clusters_num */
    Py_ssize_t i,j;

    if (!PyArg_ParseTuple(args, "O!O!ii", &PyList_Type, &centroids_list, &PyList_Type, &origin_vector_list, &max_iter, &clusters_num)) {
        return NULL;
    }

    n = PyList_Size(PyList_GetItem(centroids_list, 0)); /* vector_len*/
    vector_len = n;
    
    centroids = (double**) calloc(clusters_num, n*sizeof(double));
    for ( i=0; i<clusters_num; i++) {
        centroids[i] = (double*) calloc(n,sizeof(double));
        assert(centroids[i] != NULL);
    }
    

    for (i = 0; i < clusters_num; i++){
        for (j=0; j < n; j++) {
            centroids[i][j] = PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(centroids_list, i), j)); /*CONVERSION*/
        }
    }

    m = PyList_Size(origin_vector_list); /* vector_num */
    vector_num = m;
    

    vector_list = (double**) calloc(m, n*sizeof(double));
    for ( i=0; i<m; i++) {
        vector_list[i] = (double*) calloc(n,sizeof(double));
        assert(centroids[i] != NULL);
    }

    for (i = 0; i < m; i++){
        for (j=0; j < n; j++) {
            vector_list[i][j] = PyFloat_AsDouble(PyList_GetItem(PyList_GetItem(origin_vector_list, i), j)); /*CONVERSION*/
        }
    } /* Now global vector_list is up to date */

    calccentroids(max_iter);

    PyObject *output_centroids = PyList_New(0);
    for (i = 0; i<clusters_num; i++) {
        PyObject * centroid = PyList_New(0);

        for(j=0; j<n;j++) {
            PyList_Append(centroid, Py_BuildValue("d",centroids[i][j]));
        }

        PyList_Append(output_centroids, centroid);
    }
    
    return output_centroids;
}


static PyMethodDef kmeansMethods[] = {
    {"fit",
     (PyCFunction) fit,
     METH_VARARGS,
     PyDoc_STR("The final centroids produced by the Kmeans algorithm")},
     {NULL, NULL, 0, NULL}
};

static struct PyModuleDef moduledef = {
    PyModuleDef_HEAD_INIT,
    "mykmeanssp",
    NULL,
    -1,
    kmeansMethods
};


PyMODINIT_FUNC
PyInit_mykmeanssp(void)
{
    PyObject* m;
    m = PyModule_Create(&moduledef);
    if (!m)  {
        return NULL;
    }
    return m;
};