#include <iostream>
#include <stdlib.h>
#include <time.h>
#include <assert.h>
#include <string.h>

#include <GL/glut.h>
#include <libxml/parser.h>
#include <libxml/tree.h>

#include "graphics/glutility.h"
#include "level.h"
#include "math/perlin.h"
#include "math/math-utility.h"
#include "object/level-platform.h"
#include "physics/physics_instance.h"
#include "world.h"
#include "graphics/render_shepherd.h"

using namespace std;

//
// Level class
//


Level::Level()
{
    world = NULL;

#ifdef __GAME__
    b2d_world = NULL;
#endif //__GAME__
}

Level::Level(World *w)
{
    world = w;

#ifdef __GAME__
    b2d_world = NULL;
#endif //__GAME__
}

//clean up after ourselves
Level::~Level()
{
	for(int i = 0; i < platforms.size(); i++) {
		delete platforms[i];
	}

	for(int i = 0; i < crates.size(); i++) {
		delete crates[i];
	}
}

void Level::init()
{

    //cout<<w->loaded_meshes.size()<<endl;
    //assert(w->loaded_meshes.size() >= 2);
    //assert(w->loaded_physics_shapes.size() >= 1);
    //StaticMesh *platform_mesh = w->loaded_meshes[1];
    //PhysicsShape2D *platform_physics = w->loaded_physics_shapes[0];
	for(int i = 0; i < platforms.size(); i++) {

#ifdef __GAME__
		platforms[i]->set_b2d_world(b2d_world);
#endif //__GAME__

		//platforms[i]->set_mesh(platform_mesh);
		//platforms[i]->set_physics_shape(platform_physics);

		platforms[i]->init();
	}

    //StaticMesh *crate_mesh = w->loaded_meshes[0];

	for(int i = 0; i < crates.size(); i++) {
#ifdef __GAME__
		crates[i]->set_b2d_world(b2d_world);
#endif //__GAME__
		//crates[i]->set_mesh(crate_mesh);
		crates[i]->init();
	}
}

void Level::simulate(float frame_time, float game_time)
{
	for(int i = 0; i < platforms.size(); i++) {
		LevelPlatform2D *lp = platforms[i];
		lp->simulate(frame_time);
	}

	for(int i = 0; i < crates.size(); i++) {
		Crate2D *cp = crates[i];
		cp->simulate(frame_time);
	}
}

bool Level::read_from_file(const char *filename, RenderShepherd *rs)
{
#ifdef __GAME__
    assert(rs);
    FILE *fp = fopen(filename, "rb");
    assert(fp);

    //read the file header
    LevelFileHeader level_header;
    memset(&level_header, 0, sizeof(LevelFileHeader));
    fread(&level_header, sizeof(LevelFileHeader), 1, fp);

    assert(level_header.magic = 'BPKG');

    //now read the rest of the file
    int remaining_size = level_header.file_size - sizeof(LevelFileHeader);
    unsigned char *level_data = (unsigned char *)malloc(remaining_size);
    fread(level_data, remaining_size, 1, fp);

    fclose(fp);

    //ok, the file was successfully read, now we have to patch up
    //all the data
    unsigned char *data_ptr = level_data;
    int num_platforms = ((int *)data_ptr)[0];
    int num_bricks = ((int *)data_ptr)[1];
    int num_meshes = ((int *)data_ptr)[2];
    int num_physics_shapes = ((int *)data_ptr)[3];
    int num_textures = ((int *)data_ptr)[4];
    data_ptr += sizeof(int) * 5;

    cout<<"num_platforms: "<<num_platforms<<endl;
    cout<<"num_bricks: "<<num_bricks<<endl;
    cout<<"num_meshes: "<<num_meshes<<endl;
    cout<<"num_physics_shapes: "<<num_physics_shapes<<endl;
    cout<<"num_textures: "<<num_textures<<endl;

    //level platforms first
    LevelPlatformFileData *lpfd = (LevelPlatformFileData *)data_ptr;
    data_ptr += sizeof(LevelPlatformFileData) * num_platforms;

    for(int i = 0; i < num_platforms; i++)
    {
        cout<<"level platform"<<endl;
        cout<<"\tbrick id: "<<lpfd[i].brick_idx<<endl;
        cout<<"\tpos: "<<lpfd[i].position[0]<<", "<<lpfd[i].position[1]<<endl;
        cout<<"\tscale: "<<lpfd[i].scale[0]<<", "<<lpfd[i].scale[1]<<endl;

        LevelPlatform2D *platform = new LevelPlatform2D;
        platform->set_pos(lpfd[i].position);
        platform->set_scale(lpfd[i].scale);

        //HACK
        platform->set_mesh((StaticMesh *)lpfd[i].brick_idx);

        platforms.push_back(platform);
    }

    //bricks next
    BrickFileData *bfd = (BrickFileData *)data_ptr;
    data_ptr += sizeof(BrickFileData) * num_bricks;
    std::vector<BrickFileData> bricks;
    for(int i = 0; i < num_bricks; i++)
    {
        cout<<endl<<"brick"<<endl;
        cout<<"\thash_id: "<<bfd[i].hash_id<<endl;
        cout<<"\tmesh_idx: "<<bfd[i].mesh_idx<<endl;
        cout<<"\tphys_idx: "<<bfd[i].physics_idx<<endl;

        bricks.push_back(bfd[i]);
    }

    //meshes
    std::vector<StaticMesh *> meshes;
    for(int i = 0; i < num_meshes; i++)
    {
        //terrible... should really have a mesh header
        int hash_id = ((int *)data_ptr)[0];
        int num_materials = ((int *)data_ptr)[1];
        int num_vertices = ((int *)data_ptr)[2];
        int num_faces = ((int *)data_ptr)[3];
        data_ptr += sizeof(int) * 4;

        MaterialFileData *mfd = (MaterialFileData *)data_ptr;
        for(int j = 0; j < num_materials; j++)
        {
            data_ptr += sizeof(Float3); //diffuse_rgb

            int num_textures = *(int *)(data_ptr);
            data_ptr += sizeof(int);    //num_textures

            //int *tmp = (int *)data_ptr;
            //mfd->tex_idx = (int *)data_ptr;
            data_ptr += sizeof(int) * num_textures;
        }
        //data_ptr += sizeof(MaterialFileData) * num_materials;

        VertexFileData *vfd = (VertexFileData *)data_ptr;
        data_ptr += sizeof(VertexFileData) * num_vertices;

        FaceFileData *ffd = (FaceFileData *)data_ptr;
        data_ptr += sizeof(FaceFileData) * num_faces;

        StaticMeshFileData smfd;
        smfd.hash_id = hash_id;
        smfd.num_materials = num_materials;
        smfd.num_vertices = num_vertices;
        smfd.num_faces = num_faces;
        smfd.materials = mfd;
        smfd.vertices = vfd;
        smfd.faces = ffd;

        StaticMesh *mesh = new StaticMesh;
        mesh->read_from_file_data(&smfd);
        mesh->fixup_materials(world);
        meshes.push_back(mesh);

        rs->register_mesh(mesh);

        cout<<endl<<"mesh"<<endl;
        cout<<"\thash_id: "<<(unsigned long)hash_id<<endl;
        cout<<"\tnum_materials: "<<num_materials<<endl;
        cout<<"\tnum_vertices: "<<num_vertices<<endl;
        cout<<"\tnum_faces: "<<num_faces<<endl;
    }

    //physics shapes
    std::vector<PhysicsShape2D *> physics_shapes;
    for(int i = 0; i < num_physics_shapes; i++)
    {
        int hash_id = ((int *)data_ptr)[0];
        int num_verts = ((int *)data_ptr)[1];
        data_ptr += sizeof(int) * 2;

        Float2 *verts = (Float2 *)data_ptr;
        data_ptr += sizeof(Float2) * num_verts;

        cout<<"\thash_id: "<<(unsigned long)hash_id<<endl;
        cout<<"\tnum_verts: "<<num_verts<<endl;

        PhysicsShape2D *physics_shape = new PhysicsShape2D;
        physics_shape->set_vertices(num_verts, verts);

        physics_shapes.push_back(physics_shape);
    }

    //textures
    for(int i = 0; i < num_textures; i++)
    {
        TextureFileData tfd;
        tfd.hash_id = ((int *)data_ptr)[0];
        data_ptr += sizeof(int);

        strcpy(tfd.file_name, (char *)data_ptr);
        cout<<tfd.file_name<<endl;
        data_ptr += sizeof(char) * 256;

        tfd.dim[0] = ((int *)data_ptr)[0];
        tfd.dim[1] = ((int *)data_ptr)[1];
        data_ptr += sizeof(int) * 2;

        tfd.img_size = ((int *)data_ptr)[0];
        data_ptr += sizeof(int);

        tfd.image = (unsigned char *)data_ptr;
        data_ptr += tfd.img_size;

        cout<<"img_size: "<<tfd.img_size<<endl;

        Texture *new_texture = new Texture;
        //new_texture->load();
        new_texture->load_from_file_data(tfd);

        assert(world);
        world->loaded_textures.push_back(new_texture);
        rs->register_texture(new_texture);
    }

    //fix up pointers
    std::vector<LevelPlatform2D *>::iterator lpi;
    for(lpi = platforms.begin(); lpi != platforms.end(); lpi++)
    {
        LevelPlatform2D *lp = *lpi;
        assert(lp);

        //HACK
        long int brick_idx = (long int)lp->get_mesh();
        BrickFileData bfd = bricks[brick_idx];

        StaticMesh *actual_mesh = meshes[bfd.mesh_idx];
        assert(actual_mesh);
        lp->set_mesh(actual_mesh);

        PhysicsShape2D *actual_physics_shape = physics_shapes[bfd.physics_idx];
        assert(actual_physics_shape);

        PhysicsInstance2D *physics_instance = new PhysicsInstance2D;
        physics_instance->set_shape(actual_physics_shape);

        physics_instance->set_position(lp->get_pos());
        lp->set_physics_instance(physics_instance);
    }
#endif //__GAME__

    return true;
    /*
	//cout<<"Level::read_from_file("<<filename<<")"<<endl;
	int file_version = -1;

	xmlDocPtr doc;
	doc = xmlReadFile(filename, NULL, 0);
	if(doc == NULL) {
		return false;
	}

	//start parsing the xml file
	xmlNode *xml_node = xmlDocGetRootElement(doc);
	if(xml_node->type == XML_ELEMENT_NODE)
	{
		//cout<<xml_node->name<<endl;
		if(!xmlStrcmp(xml_node->name, (const xmlChar *)"level"))
		{
			//read the properties
			xmlAttr *xml_attribute = NULL;
			for(xml_attribute = xml_node->properties; xml_attribute; xml_attribute = xml_attribute->next)
			{
				if(!xmlStrcmp(xml_attribute->name, (const xmlChar *)"version"))
				{
					xmlChar* value = xmlNodeListGetString(xml_node->doc, xml_attribute->children, 1);
  					//do something with value
					file_version = atoi((char *)value);
  					xmlFree(value);
				}
			}
            xmlNode *obj_node = NULL;
            for(obj_node = xml_node->children; obj_node; obj_node = obj_node->next)
            {
                assert(obj_node);

                //
                // Crate
                //
                if(!xmlStrcmp(obj_node->name, (const xmlChar *)"crate"))
                {
                    float pos[2] = { 0.0f, 0.0f };
                    float dim[2] = { 1.0f, 1.0f };

                    xmlNode *cn = NULL;
                    for(cn = obj_node->children; cn; cn = cn->next)
                    {
                        //cout<<"\tname: "<<cn->name<<endl;
                        if(!xmlStrcmp(cn->name, (const xmlChar *)"pos"))
                        {
                            char *val = strtok((char *)cn->children->content, " ");
                            pos[0] = atof(val);
                            val = strtok(NULL, " ");
                            pos[1] = atof(val);
                        }
                        else if(!xmlStrcmp(cn->name, (const xmlChar *)"scale"))
                        {
                            char *val = strtok((char *)cn->children->content, " ");
                            dim[0] = atof(val);
                            val = strtok(NULL, " ");
                            dim[1] = atof(val);
                        }
                    }

                    Crate2D *cp = new Crate2D(pos, dim);
                    crates.push_back(cp);
                    //crates[num_crates++] = cp;
                }
                else if(!xmlStrcmp(obj_node->name, (const xmlChar *)"platform"))
                {
                    float pos[2] = { 0.0f, 0.0f };
                    float dim[2] = { 1.0f, 1.0f };
                    float angle = 0.0f;
                    float angular_velocity = 0.0f;

                    char brick_fname[512];
                    brick_fname[0] = '\0';

                    xmlNode *cn = NULL;
                    for(cn = obj_node->children; cn; cn = cn->next) {
                        //cout<<"\tname: "<<cn->name<<endl;
                        if(!xmlStrcmp(cn->name, (const xmlChar *)"brick"))
                        {
                            strcpy(brick_fname, (char *)cn->children->content);
                        }
                        else if(!xmlStrcmp(cn->name, (const xmlChar *)"pos"))
                        {
                            //cout<<"pos: ";
                            char *val = strtok((char *)cn->children->content, " ");
                            pos[0] = atof(val);
                            val = strtok(NULL, " ");
                            pos[1] = atof(val);
                            //cout<<pos[0]<<", "<<pos[1]<<endl;
                        }
                        else if(!xmlStrcmp(cn->name, (const xmlChar *)"scale"))
                        {
                            //cout<<"scale: ";
                            //cout<<(char *)cn->children->content<<endl;
                            char *val = strtok((char *)cn->children->content, " ");
                            //cout<<val<<endl;
                            dim[0] = atof(val);
                            val = strtok(NULL, " ");
                            assert(val);
                            dim[1] = atof(val);
                            //cout<<dim[0]<<", "<<dim[1]<<endl;
                        }
                        else if(!xmlStrcmp(cn->name, (const xmlChar *)"angle"))
                        {
                            //cout<<"angle"<<endl;
                            angle = Math::degrees_to_radians(atof((char *)cn->children->content));
                        }
                        else if(!xmlStrcmp(cn->name, (const xmlChar *)"angular_velocity"))
                        {
                            //cout<<"angular_velocity"<<endl;
                            angular_velocity = Math::degrees_to_radians(atof((char *)cn->children->content));
                        }
                    }
                    LevelPlatform2D *lp = new LevelPlatform2D(pos, dim);
                    assert(lp);
                    if(brick_fname[0])
                    {
                        lp->read_from_file(brick_fname);
                    }
                    lp->set_angle(angle);
                    lp->set_angular_velocity(angular_velocity);
                    //platforms[num_platforms++] = lp;
                    platforms.push_back(lp);
                }
            }
		}
	}

	xmlFreeDoc(doc);

	return true;*/
}

#ifdef __EDITOR__
bool Level::write_to_file(ofstream &f)
{
    f<<"<level version=\"1\">"<<endl;

    //write out all the platforms
    for(int i = 0; i < platforms.size(); i++)
    {
        LevelPlatform2D *lp = platforms[i];
        assert(lp);

        if(!lp->write_to_file(f))
        {
            return false;
        }
    }

    //crates
    for(int i = 0; i < crates.size(); i++)
    {
        Crate2D *cp = crates[i];
        assert(cp);

        if(!cp->write_to_file(f))
        {
            return false;
        }
    }

    f<<"</level>"<<endl;

    return true;
}

void Level::add_platform(LevelPlatform2D *lp)
{
    assert(lp);
    platforms.push_back(lp);
}
#endif //__EDITOR__

//render that shit
void Level::render_gl()
{
	for(int i = 0; i < platforms.size(); i++) {
		glPushMatrix();
			platforms[i]->render_gl();
		glPopMatrix();
	}

	for(int i = 0; i < crates.size(); i++) {
		Crate2D *cp = crates[i];
		glPushMatrix();
            cp->render_gl();
        glPopMatrix();
	}
}

#ifdef __GAME__
void Level::set_b2d_world(b2World *w)
{
	assert(w);
	b2d_world = w;
}
#endif //__GAME__
