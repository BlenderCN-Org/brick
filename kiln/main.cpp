#include <iostream>
#include <string.h>
#include <assert.h>
#include <stdlib.h>

//#include <GL/gl.h>
#include <SOIL/SOIL.h>

#include <mxml.h>
#include <vector>

#include "math/math-utility.h"
#include "graphics/static_mesh.h"
#include "level.h"

#define KILN_MAJOR_VERSION      1
#define KILN_MINOR_VERSION      0

#define LEVEL_PACKAGE_VERSION   1

using namespace std;
using namespace Math;

/*
enum DataType
{
    DATA_BRICK,
    DATA_CRATE,
    DATA_GEOMETRY,
    DATA_PHYSICS,
    DATA_ANIMATION,
    DATA_AUDIO,
    DATA_TEXTURE,
    DATA_SHADER,
    NUM_DATA_TYPES
};

typedef struct
{
    //unsigned long hash_id;
    DataType    type;           //type of data we are storing
    int         data_size;      //total size of the data block in bytes

    void        *data_block;    //pointer to the actual data
} DataResource;

*/

/*
typedef struct
{
    unsigned long hash_id;      //hash value from the filename / name
    unsigned long uid;          //unique identifying id - no two objects in a level will have the same id
    unsigned long type_id;      //type of data we're looking at

    int data_size;              //total size of the data chunk
    void *data;                 //pointer to the data
} DataChunk;*/

char buffer[512];
vector<string> texture_list;

/*
typedef struct
{
    LevelFileHeader header;
    DataChunk *chunks;
} LevelFileFormat;*/

Float2 mxml_read_float2(const mxml_node_t *node)
{
    assert(node);
    assert(node->next);

    Float2 val(atof(node->value.text.string),
               atof(node->next->value.text.string));

    return val;
}

Float3 mxml_read_float3(const mxml_node_t *node)
{
    assert(node);
    assert(node->next);
    assert(node->next->next);

    Float3 val(atof(node->value.text.string),
               atof(node->next->value.text.string),
               atof(node->next->next->value.text.string));

    return val;
}

bool read_physics_file(const char *fname, PhysicsShapeFileData &file_data)
{
    cout<<endl<<endl;
    cout<<"loading "<<fname<<endl;

    file_data.hash_id = Math::hash((unsigned char *)fname);
    cout<<"\thash value:"<<file_data.hash_id<<endl;

    FILE *fp = fopen(fname, "r");
    assert(fp);

    mxml_node_t *tree = mxmlLoadFile(NULL, fp, MXML_TEXT_CALLBACK);
    assert(tree);

    fclose(fp);

    mxml_node_t *node;
    node = mxmlFindElement(tree,
                           tree,
                           "physics_shape",
                           "version",
                           NULL,
                           MXML_DESCEND);

    if(node)
    {
        int version = atoi(mxmlElementGetAttr(node, "version"));
        cout<<"\tphysics_shape version: "<<version<<endl;

        std::vector<Float2> vertices;

        mxml_node_t *pos_node = NULL;
        mxml_node_t *start_node = tree;
        do
        {
            pos_node = mxmlFindElement(start_node,
                                       tree,
                                       "pos",
                                       NULL,
                                       NULL,
                                       MXML_DESCEND);
            if(pos_node)
            {
                assert(pos_node->child);
                Float2 pos = mxml_read_float2(pos_node->child);
                vertices.push_back(pos);
            }
            start_node = pos_node;
        } while(pos_node);

        file_data.num_verts = vertices.size();
        file_data.verts = new Float2[vertices.size()];

        //check for concavity, and possibly reverse the order
        //...the lazy version - just check the first 3 verts... >:)
        bool reverse_order = false;
        Float2 v0 = vertices[1] - vertices[0];
        Float2 v1 = vertices[2] - vertices[0];

        Float3 cross = Float3(v0, 0.0) ^ Float3(v1, 0.0f);
        reverse_order = (cross[2] < 0.0f);

        std::vector<Float2>::iterator it;
        int vert_idx = 0;
        if(reverse_order)
        {
            cout<<"BACKWARDS PHYSICS VERTS"<<endl;
            vert_idx = file_data.num_verts - 1;
        }
        for(it = vertices.begin(); it != vertices.end(); it++)
        {
            Float2 p = (*it);
            file_data.verts[vert_idx] = p;
            reverse_order ? vert_idx-- : vert_idx++;
            //cout<<"\t"<<p[0]<<", "<<p[1]<<endl;
        }
    }

    return true;
}

bool read_geometry_file(const char *fname, StaticMeshFileData &file_data)
{
    cout<<endl<<endl;
    cout<<"loading "<<fname<<endl;

    file_data.hash_id = Math::hash((unsigned char *)fname);
    cout<<"\thash value:"<<file_data.hash_id<<endl;

    FILE *fp = fopen(fname, "r");
    assert(fp);

    mxml_node_t *tree = mxmlLoadFile(NULL, fp, MXML_TEXT_CALLBACK);
    assert(tree);

    fclose(fp);

    mxml_node_t *node;
    node = mxmlFindElement(tree,
                           tree,
                           "static_mesh",
                           "version",
                           NULL,
                           MXML_DESCEND);

    if(node)
    {
        int version = atoi(mxmlElementGetAttr(node, "version"));
        cout<<"\tstatic_mesh version: "<<version<<endl;

        std::vector<MaterialFileData>   material_data;
        std::vector<FaceFileData>       face_data;
        std::vector<VertexFileData>     vert_data;

        //read all the materials
        mxml_node_t *material_node = NULL;
        mxml_node_t *start_node = tree;
        do
        {
            material_node = mxmlFindElement(start_node,
                                            tree,
                                            "material",
                                            NULL,
                                            NULL,
                                            MXML_DESCEND);

            if(material_node)
            {
                MaterialFileData mfd;

                //cout<<"material"<<endl;
                mxml_node_t *diff_node = mxmlFindElement(material_node,
                                                         material_node,
                                                         "diffuse_color",
                                                         NULL,
                                                         NULL,
                                                         MXML_DESCEND);

                if(diff_node)
                {
                    Float3 diff_color = mxml_read_float3(diff_node->child);
                    mfd.diffuse_rgb = diff_color;
                    //cout<<diff_color[0]<<", "<<diff_color[1]<<", "<<diff_color[2]<<endl;
                }

                //read all the textures used by this material
                mxml_node_t *tex_node = NULL;
                mxml_node_t *t_start_node = material_node;

                //int num_textures = 0;
                vector<int> texture_hash_ids;
                do
                {
                    tex_node = mxmlFindElement(t_start_node,
                                               material_node,
                                               "texture",
                                               NULL,
                                               NULL,
                                               MXML_DESCEND);


                    if(tex_node)
                    {
                        //cout<<tex_node->child->value.text.string<<endl;
                        vector<string>::iterator tsi;
                        vector<string>::iterator ts_begin = texture_list.begin();
                        vector<string>::iterator ts_end = texture_list.end();
                        bool found = false;
                        int tex_id = -1;
                        for(tsi = ts_begin; tsi != ts_end; tsi++)
                        {
                            tex_id++;
                            string tex_string = *tsi;
                            if(!strcmp(tex_string.c_str(), tex_node->child->value.text.string))
                            {
                                found = true;
                                break;
                            }
                        }
                        if(!found)
                        {
                            string s = tex_node->child->value.text.string;
                            texture_list.push_back(s);
                            tex_id = texture_list.size();
                        }
                        texture_hash_ids.push_back(tex_id);//Math::hash((unsigned char *)tex_node->child->value.text.string));
                    }
                    t_start_node = tex_node;
                } while(tex_node);

                //allocate material texture list and populate
                mfd.tex_idx = (int *)malloc(sizeof(int) * texture_hash_ids.size());
                mfd.num_textures = texture_hash_ids.size();
                for(int i = 0; i < mfd.num_textures; i++)
                {
                    mfd.tex_idx[i] = texture_hash_ids[i];
                }

                material_data.push_back(mfd);
            }

            start_node = material_node;
        } while(material_node);


        //read all the vertices
        mxml_node_t *vert_node = NULL;
        start_node = tree;
        do
        {
            vert_node = mxmlFindElement(start_node,
                                        tree,
                                        "vertex",
                                        NULL,
                                        NULL,
                                        MXML_DESCEND);

            if(vert_node)
            {
                //cout<<"vertex"<<endl;
                VertexFileData vfd;

                mxml_node_t *pos_node = mxmlFindElement(vert_node,
                                                        vert_node,
                                                        "vert_pos",
                                                        NULL,
                                                        NULL,
                                                        MXML_DESCEND);
                assert(pos_node);
                Float3 vert_pos = mxml_read_float3(pos_node->child);
                vfd.position = vert_pos;
                //cout<<"\tpos: "<<vert_pos[0]<<", "<<vert_pos[1]<<", "<<vert_pos[2]<<endl;

                mxml_node_t *norm_node = mxmlFindElement(vert_node,
                                                         vert_node,
                                                         "normal",
                                                         NULL,
                                                         NULL,
                                                         MXML_DESCEND);
                assert(norm_node);
                Float3 vert_normal = mxml_read_float3(norm_node->child);
                vfd.normal = vert_normal;
                //cout<<"\tnorm: "<<vert_normal[0]<<", "<<vert_normal[1]<<", "<<vert_normal[2]<<endl;

                vert_data.push_back(vfd);
            }

            start_node = vert_node;
        } while(vert_node);


        //read all the faces
        mxml_node_t *face_node = NULL;
        start_node = tree;
        do
        {
            face_node = mxmlFindElement(start_node,
                                        tree,
                                        "face",
                                        NULL,
                                        NULL,
                                        MXML_DESCEND);

            if(face_node)
            {
                //cout<<"face"<<endl;
                FaceFileData ffd;

                mxml_node_t *face_idx_node = mxmlFindElement(face_node,
                                                            face_node,
                                                            "face_idx",
                                                            NULL,
                                                            NULL,
                                                            MXML_DESCEND);

                assert(face_idx_node);
                int face_idx = atoi(face_idx_node->child->value.text.string);
                ffd.face_idx = face_idx;
                //cout<<"\tface_idx: "<<face_idx<<endl;

                mxml_node_t *mat_idx_node = mxmlFindElement(face_node,
                                                            face_node,
                                                            "mat_idx",
                                                            NULL,
                                                            NULL,
                                                            MXML_DESCEND);

                int mat_idx = atoi(mat_idx_node->child->value.text.string);
                ffd.mat_idx = mat_idx;
                //cout<<"\tmat_idx: "<<mat_idx<<endl;

                //3 vertices per face
                int v_idx[3] = { -1, -1, -1 };
                Float3 col[3];
                Float2 uvs[3];

                mxml_node_t *vidx_node = face_node;
                mxml_node_t *uv_node = face_node;
                mxml_node_t *col_node = face_node;
                for(int i = 0; i < 3; i++)
                {
                    //uvs
                    uv_node = mxmlFindElement(uv_node,
                                              face_node,
                                              "uv",
                                              NULL,
                                              NULL,
                                              MXML_DESCEND);
                    assert(uv_node);
                    uvs[i] = mxml_read_float2(uv_node->child);
                    ffd.uvs[i] = uvs[i];

                    //vertex color
                    col_node = mxmlFindElement(col_node,
                                              face_node,
                                              "col",
                                              NULL,
                                              NULL,
                                              MXML_DESCEND);
                    assert(col_node);
                    col[i] = mxml_read_float3(col_node->child);
                    ffd.color[i] = col[i];

                    //vertex index
                    vidx_node = mxmlFindElement(vidx_node,
                                                face_node,
                                                "v_idx",
                                                NULL,
                                                NULL,
                                                MXML_DESCEND);
                    assert(vidx_node);
                    v_idx[i] = atoi(vidx_node->child->value.text.string);
                    ffd.vert_idx[i] = v_idx[i];

                    //cout<<"\tv_idx: "<<v_idx[i]<<endl;
                    //cout<<"\tuv: "<<uvs[i][0]<<", "<<uvs[i][1]<<endl;
                    //cout<<"\tcol: "<<col[i][0]<<", "<<col[i][1]<<", "<<col[i][2]<<endl;
                }

                //face normal
                mxml_node_t *norm_node = mxmlFindElement(face_node,
                                                         face_node,
                                                         "normal",
                                                         NULL,
                                                         NULL,
                                                         MXML_DESCEND);
                assert(norm_node);
                Float3 face_normal = mxml_read_float3(norm_node->child);
                ffd.face_normal = face_normal;
                //cout<<"\tnorm: "<<face_normal[0]<<", "<<face_normal[1]<<", "<<face_normal[2]<<endl;

                face_data.push_back(ffd);
            }

            start_node = face_node;
        } while(face_node);

        file_data.num_materials = material_data.size();
        file_data.num_vertices = vert_data.size();
        file_data.num_faces = face_data.size();

        file_data.materials = new MaterialFileData[file_data.num_materials];
        file_data.vertices = new VertexFileData[file_data.num_vertices];
        file_data.faces = new FaceFileData[file_data.num_faces];

        for(int i = 0; i < file_data.num_materials; i++)
        {
            file_data.materials[i] = material_data[i];
        }
        for(int i = 0; i < file_data.num_vertices; i++)
        {
            file_data.vertices[i] = vert_data[i];
        }
        for(int i = 0; i < file_data.num_faces; i++)
        {
            file_data.faces[i] = face_data[i];
        }
    }

    return true;
}

bool read_brick_file(const char *fname, BrickFileData &file_data, StaticMeshFileData &mesh_data, PhysicsShapeFileData &physics_data)
{
    cout<<endl<<endl;
    cout<<"loading "<<fname<<endl;

    file_data.hash_id = Math::hash((unsigned char *)fname);
    cout<<"\thash value:"<<file_data.hash_id<<endl;

    FILE *fp = fopen(fname, "r");
    assert(fp);

    mxml_node_t *tree = mxmlLoadFile(NULL, fp, MXML_TEXT_CALLBACK);
    assert(tree);
    fclose(fp);

    //parse the tree
    mxml_node_t *node;
    node = mxmlFindElement(tree,
                           tree,
                           "brick",
                           "version", //"attr",
                           NULL, //"value",
                           MXML_DESCEND);

    if(node)
    {
        int version = atoi(mxmlElementGetAttr(node, "version"));
        cout<<"\tbrick version: "<<version<<endl;

        mxml_node_t *geo_node = NULL;

        geo_node = mxmlFindElement(tree,
                                   tree,
                                   "geometry",
                                   NULL,
                                   NULL,
                                   MXML_DESCEND);

        if(geo_node)
        {
            assert(geo_node->child);
            bool result = read_geometry_file(geo_node->child->value.text.string, mesh_data);
            assert(result);
        }
        else
        {
            cerr<<"Failed to find geometry file reference!"<<endl;
            return false;
        }

        //load the physics data
        mxml_node_t *phys_node = mxmlFindElement(tree,
                                                 tree,
                                                 "physics",
                                                 NULL,
                                                 NULL,
                                                 MXML_DESCEND);
        if(phys_node)
        {
            assert(phys_node->child);
            bool result = read_physics_file(phys_node->child->value.text.string, physics_data);
            assert(result);
        }
        else
        {
            cerr<<"Failed to find physics file reference!"<<endl;
            return false;
        }
    }
    return true;
}

bool read_texture_file(const char *fname, TextureFileData &tfd)
{
    cout<<endl<<endl;
    cout<<"loading "<<fname<<endl;

    strcpy(tfd.file_name, fname);
    tfd.hash_id = Math::hash((unsigned char *)fname);
    cout<<"\thash value:"<<tfd.hash_id<<endl;

    int width, height, channels;
    unsigned char *image = SOIL_load_image(fname, &width, &height, &channels, SOIL_LOAD_RGBA);
    assert(image);

    channels = 4; //we force everything to RGBA anyway...

    cout<<"width: "<<width<<endl;
    cout<<"height: "<<height<<endl;
    cout<<"channels: "<<channels<<endl;

    tfd.dim[0] = width;
    tfd.dim[1] = height;

    //copy image data to TextureFileData struct
    tfd.img_size = width * height * channels * sizeof(unsigned char);
    tfd.image = (unsigned char *)malloc(tfd.img_size);
    memcpy(tfd.image, image, tfd.img_size);

    SOIL_free_image_data(image);

    return true;
}

void read_level_xml(const char *fname, LevelFileData &file_data)
{
    cout<<"loading level file "<<fname<<"..."<<endl;

    std::vector<char *> bricks_to_load;

    FILE *fp = fopen(fname, "r");
    assert(fp);

    mxml_node_t *tree = mxmlLoadFile(NULL, fp, MXML_TEXT_CALLBACK);
    assert(tree);

    fclose(fp);

    //parse tree
    mxml_node_t *node;

    node = mxmlFindElement(tree,
                           tree,
                           "level",
                           "version", //"attr",
                           NULL, //"value",
                           MXML_DESCEND);
    if(node)
    {
        //int version = mxmlGetInteger(node);
        int version = atoi(mxmlElementGetAttr(node, "version"));
        cout<<"\tlevel version: "<<version<<endl;

        //ok, now, parse and process all the children nodes
        //start with platforms...
        mxml_node_t *platform_node = NULL;
        mxml_node_t *start_node = tree;

        std::vector<LevelPlatformFileData> platforms;
        do
        {
            platform_node = mxmlFindElement(start_node,
                                            tree,
                                            "platform",
                                            NULL,
                                            NULL,
                                            MXML_DESCEND);

            if(platform_node)
            {
                LevelPlatformFileData platform_data;

                mxml_node_t *brick_node = mxmlFindElement(platform_node,
                                                          platform_node,
                                                          "brick",
                                                          NULL,
                                                          NULL,
                                                          MXML_DESCEND);

                assert(brick_node);
                char *tmp = brick_node->child->value.text.string;
                std::vector<char *>::iterator it;
                bool match_found = false;
                int brick_idx = 0;
                for(it = bricks_to_load.begin(); it != bricks_to_load.end(); it++)
                {
                    char *array_name = *(it);
                    if(!strcmp(tmp, array_name))
                    {
                        match_found = true;
                        platform_data.brick_idx = brick_idx;
                        break;
                    }
                    brick_idx++;
                }
                if(!match_found)
                {
                    bricks_to_load.push_back(tmp);
                    platform_data.brick_idx = bricks_to_load.size() - 1;
                }

                //position
                mxml_node_t *pos_node = mxmlFindElement(platform_node,
                                                        platform_node,
                                                        "pos",
                                                        NULL,
                                                        NULL,
                                                        MXML_DESCEND);
                assert(pos_node);
                platform_data.position = mxml_read_float2(pos_node->child);

                platform_data.scale = Float2(1.0f, 1.0f);
                platform_data.angle = 0.0f;

                platforms.push_back(platform_data);
            }

            start_node = platform_node;
        } while(platform_node != NULL);

        file_data.num_platforms = platforms.size();
        file_data.platforms = new LevelPlatformFileData[file_data.num_platforms];
        for(int i = 0; i < file_data.num_platforms; i++)
        {
            file_data.platforms[i] = platforms[i];
        }

        //now onto crates...

        //now load the actual brick files...
        file_data.num_bricks = bricks_to_load.size();
        file_data.bricks = new BrickFileData[file_data.num_bricks];

        file_data.num_meshes = bricks_to_load.size();
        file_data.meshes = new StaticMeshFileData[file_data.num_meshes];

        file_data.num_physics_shapes = bricks_to_load.size();
        file_data.physics_shapes = new PhysicsShapeFileData[file_data.num_physics_shapes];

        std::vector<char *>::iterator it;
        int brick_idx = 0;
        int mesh_idx = 0;
        int phys_idx = 0;

        for(it = bricks_to_load.begin(); it != bricks_to_load.end(); it++)
        {
            BrickFileData bfd;
            StaticMeshFileData mfd;
            PhysicsShapeFileData pfd;

            read_brick_file(*(it), bfd, mfd, pfd);
            bfd.mesh_idx = mesh_idx;
            bfd.physics_idx = phys_idx;

            file_data.meshes[mesh_idx++] = mfd;
            file_data.physics_shapes[phys_idx++] = pfd;
            file_data.bricks[brick_idx++] = bfd;
        }

        file_data.num_textures = texture_list.size();
        file_data.textures = new TextureFileData[file_data.num_textures];

        cout<<endl<<"loading textures..."<<endl;

        //load all textures
        vector<string>::iterator tsi;
        vector<string>::iterator tsi_begin = texture_list.begin();
        vector<string>::iterator tsi_end = texture_list.end();
        int tex_idx = 0;
        for(tsi = tsi_begin; tsi != tsi_end; tsi++)
        {
            string ts = *tsi;

            TextureFileData tfd;
            read_texture_file(ts.c_str(), tfd);

            file_data.textures[tex_idx++] = tfd;
        }
    }
    else
    {
        cerr<<"Invalid level file!"<<endl;
    }
}

int calculate_file_size(LevelFileData &pkg_data)
{
    int file_size = sizeof(LevelFileHeader);
    file_size += sizeof(int); //num_platforms
    file_size += sizeof(int); //num_bricks
    file_size += sizeof(int); //num_meshes
    file_size += sizeof(int); //num_physics_shapes
    file_size += sizeof(int); //num_textures

    file_size += sizeof(LevelPlatformFileData) * pkg_data.num_platforms;
    file_size += sizeof(BrickFileData) * pkg_data.num_bricks;

    for(int i = 0; i < pkg_data.num_meshes; i++)
    {
        file_size += sizeof(int); //hash_id
        file_size += sizeof(int); //num_materials
        file_size += sizeof(int); //num_verts
        file_size += sizeof(int); //num_faces

        //file_size += sizeof(MaterialFileData) * pkg_data.meshes[i].num_materials;
        for(int j = 0; j < pkg_data.meshes[i].num_materials; j++)
        {
            file_size += sizeof(Float3);    //diffuse_color
            file_size += sizeof(int);       //num_textures

            file_size += sizeof(int) * pkg_data.meshes[i].materials[j].num_textures;
        }

        file_size += sizeof(VertexFileData) * pkg_data.meshes[i].num_vertices;
        file_size += sizeof(FaceFileData) * pkg_data.meshes[i].num_faces;
    }

    for(int i = 0; i < pkg_data.num_physics_shapes; i++)
    {
        file_size += sizeof(int); //hash_id
        file_size += sizeof(int); //num_verts

        file_size += sizeof(Float2) * pkg_data.physics_shapes[i].num_verts;
    }

    //textures
    for(int i = 0; i < pkg_data.num_textures; i++)
    {
        file_size += sizeof(int);           //hash_id
        file_size += sizeof(char) * 256;    //filename
        file_size += sizeof(int) * 2;       //dim
        file_size += sizeof(int);           //img_size

        file_size += pkg_data.textures[i].img_size;
    }

    cout<<"file size: "<<file_size / 1024<<" kB ("<<file_size<<" bytes)."<<endl;
    return file_size;
}

void write_level_package(const char *fname, LevelFileData &pkg_data)
{
    cout<<endl<<"writing level package "<<fname<<"..."<<endl;
    //compute the total size of the
    LevelFileHeader header;
    header.magic = 'BPKG';
    header.version = LEVEL_PACKAGE_VERSION;
    header.header_size = sizeof(LevelFileHeader);
    header.file_size = calculate_file_size(pkg_data);

    FILE *fp = fopen(fname, "wb");
    assert(fp);

    fwrite(&header, sizeof(LevelFileHeader), 1, fp);

    //write the actual level data
    //TODO: separate this out into a header for easy io
    fwrite(&pkg_data.num_platforms, sizeof(int), 1, fp);
    fwrite(&pkg_data.num_bricks, sizeof(int), 1, fp);
    fwrite(&pkg_data.num_meshes, sizeof(int), 1, fp);
    fwrite(&pkg_data.num_physics_shapes, sizeof(int), 1, fp);
    fwrite(&pkg_data.num_textures, sizeof(int), 1, fp);

    //write all level platforms
    fwrite(pkg_data.platforms, sizeof(LevelPlatformFileData), pkg_data.num_platforms, fp);

    //write all bricks
    fwrite(pkg_data.bricks, sizeof(BrickFileData), pkg_data.num_bricks, fp);

    //write all meshes
    for(int i = 0; i < pkg_data.num_meshes; i++)
    {
        //this should really be separated out into a header for
        //easy io with one call to fwrite
        fwrite(&pkg_data.meshes[i].hash_id, sizeof(int), 1, fp);
        fwrite(&pkg_data.meshes[i].num_materials, sizeof(int), 1, fp);
        fwrite(&pkg_data.meshes[i].num_vertices, sizeof(int), 1, fp);
        fwrite(&pkg_data.meshes[i].num_faces, sizeof(int), 1, fp);

        //fwrite(pkg_data.meshes[i].materials, sizeof(MaterialFileData), pkg_data.meshes[i].num_materials, fp);
        //this is equally terrible - materials should be their own resource, not embedded here
        for(int j = 0; j < pkg_data.meshes[i].num_materials; j++)
        {
            fwrite(&pkg_data.meshes[i].materials[j].diffuse_rgb, sizeof(Float3), 1, fp);
            fwrite(&pkg_data.meshes[i].materials[j].num_textures, sizeof(int), 1, fp);
            fwrite(pkg_data.meshes[i].materials[j].tex_idx, sizeof(int), pkg_data.meshes[i].materials[j].num_textures, fp);
        }
        fwrite(pkg_data.meshes[i].vertices, sizeof(VertexFileData), pkg_data.meshes[i].num_vertices, fp);
        fwrite(pkg_data.meshes[i].faces, sizeof(FaceFileData), pkg_data.meshes[i].num_faces, fp);
    }

    //write all physics shapes
    for(int i = 0; i < pkg_data.num_physics_shapes; i++)
    {
        //header
        fwrite(&pkg_data.physics_shapes[i].hash_id, sizeof(int), 1, fp);
        fwrite(&pkg_data.physics_shapes[i].num_verts, sizeof(int), 1, fp);

        //data
        fwrite(pkg_data.physics_shapes[i].verts, sizeof(Float2), pkg_data.physics_shapes[i].num_verts, fp);
    }

    //write all texture data
    for(int i = 0; i < pkg_data.num_textures; i++)
    {
        //header
        fwrite(&pkg_data.textures[i].hash_id, sizeof(int), 1, fp);
        fwrite(&pkg_data.textures[i].file_name, sizeof(char), 256, fp);
        fwrite(&pkg_data.textures[i].dim, sizeof(int), 2, fp);

        fwrite(&pkg_data.textures[i].img_size, sizeof(int), 1, fp);
        fwrite(pkg_data.textures[i].image, pkg_data.textures[i].img_size, 1, fp);
    }

    int fsize_written = ftell(fp);
    cout<<"file size written: "<<fsize_written<<" bytes"<<endl;
    if(fsize_written != header.file_size)
    {
        cerr<<"\033[1;31mERROR: file sizes don't match!"<<endl;
    }

    fclose(fp);
}

int main(int argc, char **argv)
{
    buffer[0] = '\0';

    LevelFileData level_data;

    cout<<"kiln v"<<KILN_MAJOR_VERSION<<"."<<KILN_MINOR_VERSION<<endl;
    for(int i = 0; i < argc; i++)
    {
        cout<<argv[i]<<endl;
    }

    if(argc > 1)
    {
        read_level_xml(argv[1], level_data);
        write_level_package("data/bin/test.pkg", level_data);
    }

    return 0;
}
