#include <iostream>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include "math/math-utility.h"
#include "level-platform.h"

using namespace std;

LevelPlatform2D::LevelPlatform2D() : Object2D()
{
	object_type = OBJECT_LEVEL_PLATFORM;
	vel[0] = vel[1] = 0.0f;
	theta = 0.0f;
	rotates = false;

	track_position = 0.0f;
	track_rate = 0.0f;
}

LevelPlatform2D::LevelPlatform2D(const float *_pos, const float *_dim) : Object2D(_pos, _dim)
{
	object_type = OBJECT_LEVEL_PLATFORM;

	vel[0] = vel[1] = 0.0f;
	theta = 0.0f;
	rotates = false;

	track_position = 0.0f;
	track_rate = 0.0f;
}

void LevelPlatform2D::init()
{
#ifdef __PHYSICS__
    physics_instance->set_b2d_world(b2d_world);
    physics_instance->init();
    physics_instance->set_position(pos);
#endif //__PHYSICS__
}

void LevelPlatform2D::simulate(const float t)
{
    return;
#ifdef __GAME__
	float x_offset = 0.0f;
	float y_offset = 0.0f;
	if(dynamic) {
		track_position += t * track_rate;
		if(track_position > 1.0f || track_position < 0.0f) {
			track_position = Math::min(Math::max(0.0f, track_position), 1.0f);
			track_rate *= -1.0f;
		}
		if(true) {
			x_offset = Math::cerp(-0.04f, 0.04f, track_position);
			//x_offset = (float)(i % 4 + 1.0f) * 0.15f * sin(game_time * 0.001f * (float)(i % 5 + 1.0f) * 0.2) * 0.05f;
			vel[0] = x_offset / t;
		} else {
			y_offset = Math::cerp(-1.0f, 1.0f, track_position);
			//y_offset = (float)(i % 4 + 1.0f) * 0.25f * sin(game_time * 0.001f * (float)(i % 5 + 1.0f) * 0.2) * 0.05f;
			vel[1] = y_offset / t;
		}
	}

	b2Vec2 body_pos = b2d_body->GetPosition();
	pos[0] = body_pos.x + x_offset;
	pos[1] = body_pos.y + y_offset;

	b2Vec2 p(pos[0], pos[1]);

	b2Vec2 body_vel(x_offset / t, y_offset / t);
	b2d_body->SetLinearVelocity(body_vel);

	if(fabs(angular_velocity) > 0.0001f)
	{
		b2d_body->SetAngularVelocity(angular_velocity);
		theta = b2d_body->GetAngle();
	}
#endif //__GAME__
}

bool LevelPlatform2D::read_from_file(const char *fname)
{
    /*
    cout<<"LevelPlatform2D::read_from_file(): "<<fname<<endl;

    int file_version = -1;

	xmlDocPtr doc;
	doc = xmlReadFile(fname, NULL, 0);
	if(doc == NULL)
	{
		return false;
	}

	//start parsing the xml file
	xmlNode *xml_node = xmlDocGetRootElement(doc);
	if(xml_node->type == XML_ELEMENT_NODE)
	{
		//cout<<xml_node->name<<endl;
		if(!xmlStrcmp(xml_node->name, (const xmlChar *)"brick"))
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

			//read children
			xmlNode *obj_node = NULL;
			for(obj_node = xml_node->children; obj_node; obj_node = obj_node->next)
			{
			    cout<<obj_node->name<<endl;
			    if(!xmlStrcmp(obj_node->name, (const xmlChar *)"physics"))
			    {
			        cout<<"<physics>"<<endl;
			        PhysicsShape2D *p_shape = new PhysicsShape2D;
                    p_shape->read_from_file((const char *)obj_node->children->content);

                    cout<<obj_node->children->content<<endl;;
                    //physics_shape = p_shape;
			    }
			}
		}
	}
    */
	return true;
}

#ifdef __EDITOR__
bool LevelPlatform2D::write_to_file(ofstream &f)
{
    f<<"\t<platform>"<<endl;
    f<<"\t\t<pos>"<<pos[0]<<" "<<pos[1]<<"</pos>"<<endl;
    f<<"\t\t<scale>"<<dim[0]<<" "<<dim[1]<<"</scale>"<<endl;
    f<<"\t</platform>"<<endl<<endl;

    return true;
}
#endif //__EDITOR__

void LevelPlatform2D::set_angle(const float a)
{
	theta = a;
}

void LevelPlatform2D::set_angular_velocity(const float v)
{
	angular_velocity = v;
}

bool LevelPlatform2D::hit_test_2d(float p[2])
{
    //rotate p into local space (TODO)

    if(p[0] < pos[0] - dim[0] / 2.0f) return false;
    if(p[0] > pos[0] + dim[0] / 2.0f) return false;
    if(p[1] < pos[1] - dim[1] / 2.0f) return false;
    if(p[1] > pos[1] + dim[1] / 2.0f) return false;

    return true;
}

void LevelPlatform2D::render_gl()
{
    glTranslatef(pos[0], pos[1], 0.0f);
    glScalef(dim[0], dim[1], 1.0f);
    mesh->render_gl();
}
