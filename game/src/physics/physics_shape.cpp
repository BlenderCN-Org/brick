#include <vector>

#include <libxml/parser.h>
#include <libxml/tree.h>

#include <GL/gl.h>

#include "graphics/material.h"

#include "physics_shape.h"

PhysicsShape2D::PhysicsShape2D()
{
    num_verts = 4;
    shape_verts = new Float2[4];

    shape_verts[3] = Float2(-0.5f, 0.5f);
    shape_verts[2] = Float2(0.5f, 0.5f);
    shape_verts[1] = Float2(0.5f, -0.5f);
    shape_verts[0] = Float2(-0.5f, -0.5f);
}

PhysicsShape2D::~PhysicsShape2D()
{
    delete shape_verts;
}

bool PhysicsShape2D::read_from_file(const char *fname)
{
    std::vector<Float2> vertices;

    int file_version = -1;

	xmlDocPtr doc;
	doc = xmlReadFile(fname, NULL, 0);
	if(doc == NULL)
	{
		return false;
	}

	//start parsing the xml file
	xmlNode *xml_node = xmlDocGetRootElement(doc);
	if(xml_node->type == XML_ELEMENT_NODE) {
	    if(!xmlStrcmp(xml_node->name, (const xmlChar *)"physics_shape")) {
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
			    else if(!xmlStrcmp(xml_attribute->name, (const xmlChar *)"type"))
			    {
			        //rigid, soft_body, etc...
			    }
			}

			xmlNode *obj_node = NULL;
            for(obj_node = xml_node->children; obj_node; obj_node = obj_node->next)
            {
                assert(obj_node);

                if(!xmlStrcmp(obj_node->name, (const xmlChar *)"pos"))
                {
                    Float2 pos;
                    char *val = strtok((char *)obj_node->children->content, " ");
                    pos[0] = atof(val);
                    val = strtok(NULL, " ");
                    pos[1] = atof(val);

                    vertices.push_back(pos);
                }
            }
	    }
	}

    //allocate the final array
	num_verts = vertices.size();
	if(shape_verts)
	{
	    delete shape_verts;
	}
	shape_verts = new Float2[vertices.size()];

	std::vector<Float2>::iterator vi;
	int i = 0;
	for(vi = vertices.begin(); vi != vertices.end(); vi++)
	{
	    Float2 v = *vi;
	    shape_verts[i++] = v;
	}

    return true;
}

void PhysicsShape2D::set_vertices(int count, Float2 *p_verts)
{
    delete shape_verts;
    num_verts = count;
    shape_verts = new Float2[count];
    for(int i = 0; i < count; i++)
    {
        shape_verts[i] = p_verts[i];
    }
}

Float2 *PhysicsShape2D::get_vertices(int &count)
{
    count = num_verts;
    return shape_verts;
}

void PhysicsShape2D::render_gl()
{
    Material m;
    m.enable_blending(false);
    m.enable_lighting(false);
    m.render_gl();

    glColor3f(1.0f, 1.0f, 1.0f);

    glBegin(GL_LINES);
    for(int i = 0; i < num_verts - 1; i++)
    {
       glVertex3f(shape_verts[i][0], shape_verts[i][1], 0.0f);
       glVertex3f(shape_verts[i + 1][0], shape_verts[i + 1][1], 0.0f);
    }
    glVertex3f(shape_verts[num_verts - 1][0], shape_verts[num_verts - 1][1], 0.0f);
    glVertex3f(shape_verts[0][0], shape_verts[0][1], 0.0f);
    glEnd();
}
