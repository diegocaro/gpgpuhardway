#include "obj_loader.h"

ObjLoader::ObjLoader() {
    g_scale=1.0f;
    scene = NULL; //empty scene
    scene_list = 0;
    num_vertices = 0;
}

unsigned int ObjLoader::getNumVertices() {return num_vertices;}

int ObjLoader::loadAsset(const char *path){
    scene = aiImportFile(path, aiProcessPreset_TargetRealtime_MaxQuality);
    if (scene) {
        get_bounding_box(&scene_min,&scene_max);
        scene_center.x = (scene_min.x + scene_max.x) / 2.0f;
        scene_center.y = (scene_min.y + scene_max.y) / 2.0f;
        scene_center.z = (scene_min.z + scene_max.z) / 2.0f;
        printf("Loaded file %s\n", path);
        g_scale =4.0/(scene_max.x-scene_min.x);
        printf("Scaling: %lf", g_scale);
        num_vertices = recursiveGetNumVertices(scene->mRootNode);
        printf("This Scene has %d vertices.\n", num_vertices);
        return 0;
    }
    return 1;
}

unsigned int ObjLoader::recursiveGetNumVertices(const struct aiNode *nd){
    unsigned int counter=0;
    unsigned int i;
    unsigned int n = 0, t;
    // draw all meshes assigned to this node
    for (; n < nd->mNumMeshes; ++n) {
        const struct aiMesh* mesh = scene-> mMeshes[nd->mMeshes[n]];
        for (t = 0; t < mesh->mNumFaces; ++t) {
            const struct aiFace* face = &mesh-> mFaces[t];
            counter+=3*face->mNumIndices;
        }
        //printf("recursiveGetNumVertices: mNumFaces %d\n", mesh->mNumFaces);
    }
    //traverse all children nodes
    for (n = 0; n < nd->mNumChildren; ++n) {
        counter+=recursiveGetNumVertices(nd-> mChildren[n]);
    }
    //printf("recursiveGetNumVertices: counter %d\n", counter);
    return counter;
}

void ObjLoader::get_bounding_box (aiVector3D* min, aiVector3D* max)
{
    aiMatrix4x4 trafo;
    aiIdentityMatrix4(&trafo);
    min->x = min->y = min->z =  1e10f;
    max->x = max->y = max->z = -1e10f;
    get_bounding_box_for_node(scene-> mRootNode,min,max,&trafo);
}

void ObjLoader::get_bounding_box_for_node (const struct aiNode* nd, \
    aiVector3D* min, aiVector3D* max, aiMatrix4x4* trafo)
{
    aiMatrix4x4 prev;
    unsigned int n = 0, t;
    prev = *trafo;
    aiMultiplyMatrix4(trafo,&nd->mTransformation);
    for (; n < nd->mNumMeshes; ++n) {
        const struct aiMesh* mesh = scene-> mMeshes[nd->mMeshes[n]];
        for (t = 0; t < mesh->mNumVertices; ++t) {
            aiVector3D tmp = mesh->mVertices[t];
            aiTransformVecByMatrix4(&tmp,trafo);
            min->x = aisgl_min(min->x,tmp.x);
            min->y = aisgl_min(min->y,tmp.y);
            min->z = aisgl_min(min->z,tmp.z);
            max->x = aisgl_max(max->x,tmp.x);
            max->y = aisgl_max(max->y,tmp.y);
            max->z = aisgl_max(max->z,tmp.z);
        }
    }

    for (n = 0; n < nd->mNumChildren; ++n) {
        get_bounding_box_for_node(nd-> mChildren[n],min,max,trafo);
    }
    *trafo = prev;
}

void ObjLoader::loadVertices(GLfloat *g_vertex_buffer_data) {
    recursiveVertexLoading(scene->mRootNode, g_vertex_buffer_data,
    0);
}
unsigned int ObjLoader::recursiveVertexLoading (const struct aiNode *nd, \
    GLfloat *g_vertex_buffer_data, unsigned int v_counter)
{
    unsigned int i;
    unsigned int n = 0, t;
    /* save all data to the vertex array, perform offset and scaling
    to reduce the computation */
    for (; n < nd->mNumMeshes; ++n) {
        const struct aiMesh* mesh = scene-> mMeshes[nd->mMeshes[n]];
        for (t = 0; t < mesh->mNumFaces; ++t) {
            const struct aiFace* face = &mesh->mFaces[t];
            for(i = 0; i < face->mNumIndices; i++) {
                int index = face->mIndices[i];
                g_vertex_buffer_data[v_counter]=
                    (mesh->mVertices[index].x-scene_center.x)*g_scale;
                g_vertex_buffer_data[v_counter+1]=
                    (mesh->mVertices[index].y-scene_center.y)*g_scale;
                g_vertex_buffer_data[v_counter+2]=
                    (mesh->mVertices[index].z-scene_center.z)*g_scale;
                v_counter+=3;
            } 
        }
    }
    //traverse all children nodes
    for (n = 0; n < nd->mNumChildren; ++n) {
        v_counter = recursiveVertexLoading(nd-> mChildren[n],
        g_vertex_buffer_data, v_counter);
    }
    return v_counter;
}

void ObjLoader::draw(const GLenum draw_mode){
    recursiveDrawing(scene->mRootNode, 0, draw_mode);
}
   
unsigned int ObjLoader::recursiveDrawing(const struct aiNode* nd,
unsigned int v_counter, const GLenum draw_mode){
    /* break up the drawing, and shift the pointer to draw different
    parts of the scene */
    unsigned int i;
    unsigned int n = 0, t;
    unsigned int total_count = v_counter;
    //printf("v_counter: %d\n",v_counter);
    // draw all meshes assigned to this node
    for (n=0; n < nd->mNumMeshes; ++n) {
        //printf("mesh number: %d\n", n);
        unsigned int count=0;
        const struct aiMesh* mesh = scene-> mMeshes[nd->mMeshes[n]];
        for (t = 0; t < mesh->mNumFaces; ++t) {
            const struct aiFace* face = &mesh-> mFaces[t];
            count+=3*face->mNumIndices;
        }
        glDrawArrays(draw_mode, total_count, count);
        total_count+=count;
    }
    v_counter = total_count;
    // draw all children nodes recursively
    for (n = 0; n < nd->mNumChildren; ++n) {
        //printf("num children: %d\n", n);
        v_counter = recursiveDrawing(nd-> mChildren[n], v_counter, draw_mode);
    }
    return v_counter;
}
   



