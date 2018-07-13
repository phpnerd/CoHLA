/*
 * Copyright (c) Tim Broenink and contributors. All rights reserved.
 * Licensed under the MIT license. See LICENSE file in the project root for details.
*/

//
// Created by Tim on 19/06/2017.
//

#ifndef COLLISION_SIM_LOADMESHFROMSTL_H
#define COLLISION_SIM_LOADMESHFROMSTL_H
#ifndef LOAD_MESH_FROM_STL_H
#define LOAD_MESH_FROM_STL_H

#include <stdio.h> //fopen
#include "Bullet3Common/b3AlignedObjectArray.h"
#include "btBulletCollisionCommon.h"
#include <bullet/HACD/hacdHACD.h>

struct MySTLTriangle {
    float normal[3];
    float vertex0[3];
    float vertex1[3];
    float vertex2[3];
};

static std::pair<btTriangleMesh *, float[3]> LoadColliderFromSTL(const char *relativeFileName) {
    btTriangleMesh *shape = 0;
    float avg[3] = {0, 0, 0};

    FILE *file = fopen(relativeFileName, "rb");
    if (file) {
        int size = 0;
        if (fseek(file, 0, SEEK_END) || (size = ftell(file)) == EOF || fseek(file, 0, SEEK_SET))
            printf("Error: Cannot access file to determine size of %s\n", relativeFileName);
        else if (size) {
//			printf("Open STL file of %d bytes\n",size);
            char *memoryBuffer = new char[size + 1];
            int actualBytesRead = fread(memoryBuffer, 1, size, file);
            if (actualBytesRead != size) {
                printf("Error reading from file %s", relativeFileName);
            } else {
                int numTriangles = *(int *) &memoryBuffer[80];

                if (numTriangles) {
                    //perform a sanity check instead of crashing on invalid triangles/STL files
                    int expectedBinaryFileSize = numTriangles * 50 + 84;
                    if (expectedBinaryFileSize != size)
                        return std::pair<btTriangleMesh *, float[3]>();

                    shape = new btTriangleMesh;
//						b3AlignedObjectArray<GLInstanceVertex>*	m_vertices;
//						int				m_numvertices;
//						b3AlignedObjectArray<int>* 		m_indices;
//						int				m_numIndices;
//						float			m_scaling[4];

                    int index = 0;

                    for (int i = 0; i < numTriangles; i++) {
                        char *curPtr = &memoryBuffer[84 + i * 50];
                        MySTLTriangle tmp;

                        memcpy(&tmp, curPtr, sizeof(MySTLTriangle));
                        avg[0] += tmp.vertex0[0] + tmp.vertex1[0] + tmp.vertex2[0];
                        avg[1] += tmp.vertex0[1] + tmp.vertex1[1] + tmp.vertex2[1];
                        avg[2] += tmp.vertex0[2] + tmp.vertex1[2] + tmp.vertex2[2];
                    }
                    for (int i = 0; i < 3; i++)
                        avg[i] /= numTriangles * 3;

                    for (int i = 0; i < numTriangles; i++) {
                        char *curPtr = &memoryBuffer[84 + i * 50];
                        MySTLTriangle tmp;
                        memcpy(&tmp, curPtr, sizeof(MySTLTriangle));

                        shape->addTriangle(btVector3((tmp.vertex0[0] - avg[0]) * 1000, (tmp.vertex0[1] - avg[1]) * 1000,
                                                     (tmp.vertex0[2] - avg[2]) * 1000),
                                           btVector3((tmp.vertex1[0] - avg[0]) * 1000, (tmp.vertex1[1] - avg[1]) * 1000,
                                                     (tmp.vertex1[2] - avg[2]) * 1000),
                                           btVector3((tmp.vertex2[0] - avg[0]) * 1000, (tmp.vertex2[1] - avg[1]) * 1000,
                                                     (tmp.vertex2[2] - avg[2]) * 1000),
                                           true);
//                        if(i%(numTriangles/100) == 0){
//                            printf("Processing %i/%i\n (%f\%)",i,numTriangles,i/numTriangles);
//                        }
                    }
                }
            }

            delete[] memoryBuffer;
        }
        fclose(file);
    }
    HACD::HACD decomposer;
    std::pair<btTriangleMesh *, float[3]> ret;
    ret.first = shape;
    ret.second[0] = avg[0] * 1000;
    ret.second[1] = avg[1] * 1000;
    ret.second[2] = avg[2] * 1000;

    return ret;
}


static std::vector<float> LoadRenderFromSTL(const char *relativeFileName) {
    std::vector<float> m;

    FILE *file = fopen(relativeFileName, "rb");
    if (file) {
        int size = 0;
        if (fseek(file, 0, SEEK_END) || (size = ftell(file)) == EOF || fseek(file, 0, SEEK_SET))
            printf("Error: Cannot access file to determine size of %s\n", relativeFileName);
        else if (size) {
//			printf("Open STL file of %d bytes\n",size);
            char *memoryBuffer = new char[size + 1];
            int actualBytesRead = fread(memoryBuffer, 1, size, file);
            if (actualBytesRead != size)
                printf("Error reading from file %s", relativeFileName);
            else {
                int numTriangles = *(int *) &memoryBuffer[80];

                if (numTriangles) {
                    //perform a sanity check instead of crashing on invalid triangles/STL files
                    int expectedBinaryFileSize = numTriangles * 50 + 84;
                    if (expectedBinaryFileSize != size)
                        return m;

                    m.resize(numTriangles * 6 * 3);
//						b3AlignedObjectArray<GLInstanceVertex>*	m_vertices;
//						int				m_numvertices;
//						b3AlignedObjectArray<int>* 		m_indices;
//						int				m_numIndices;
//						float			m_scaling[4];

                    int index = 0;


                    for (int i = 0; i < numTriangles; i++) {
                        char *curPtr = &memoryBuffer[84 + i * 50];
                        MySTLTriangle tmp;
                        memcpy(&tmp, curPtr, sizeof(MySTLTriangle));
                        m[index++] = tmp.vertex0[0] * 1000;
                        m[index++] = tmp.vertex0[1] * 1000;
                        m[index++] = tmp.vertex0[2] * 1000;
                        m[index++] = tmp.normal[0];
                        m[index++] = tmp.normal[1];
                        m[index++] = tmp.normal[2];
                        m[index++] = tmp.vertex1[0] * 1000;
                        m[index++] = tmp.vertex1[1] * 1000;
                        m[index++] = tmp.vertex1[2] * 1000;
                        m[index++] = tmp.normal[0];
                        m[index++] = tmp.normal[1];
                        m[index++] = tmp.normal[2];
                        m[index++] = tmp.vertex2[0] * 1000;
                        m[index++] = tmp.vertex2[1] * 1000;
                        m[index++] = tmp.vertex2[2] * 1000;
                        m[index++] = tmp.normal[0];
                        m[index++] = tmp.normal[1];
                        m[index++] = tmp.normal[2];
                    }
                }
            }

            delete[] memoryBuffer;
        }
        fclose(file);
    }
    return m;
}

#endif //LOAD_MESH_FROM_STL_H
#endif //COLLISION_SIM_LOADMESHFROMSTL_H