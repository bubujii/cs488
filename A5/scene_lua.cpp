// Termm--Fall 2024

//
// CS488 - Introduction to Computer Graphics
//
// scene_lua.cpp
//
// Everything that's needed to parse a scene file using Lua.
// You don't necessarily have to understand exactly everything that
// goes on here, although it will be useful to have a reasonable idea
// if you wish to add new commands to the scene format.
//
// Lua interfaces with C/C++ using a special stack. Everytime you want
// to get something from lua, or pass something back to lua (e.g. a
// return value), you need to use this stack. Thus, most of the lua_
// and luaL_ functions actually manipulate the stack. All the
// functions beginning with "lua_" are part of the Lua C API itself,
// whereas the "luaL_" functions belong to a library of useful
// functions on top of that called lauxlib.
//
// This file consists of a bunch of C function declarations which
// implement functions callable from Lua. There are also two tables
// used to set up the interface between Lua and these functions, and
// the main "driver" function, import_lua, which calls the lua
// interpreter and sets up all the state.
//
// Note that each of the function declarations follow the same format:
// they take as their only argument the current state of the lua
// interpreter, and return the number of values returned back to lua.
//
// For more information see the book "Programming In Lua," available
// online at http://www.lua.org/pil/, and of course the Lua reference
// manual at http://www.lua.org/manual/5.0/.
//
// http://lua-users.org/wiki/LauxLibDocumentation provides a useful
// documentation of the "lauxlib" functions (beginning with luaL_).
//
// -- University of Waterloo Computer Graphics Lab 2005

#include "scene_lua.hpp"

#include <iostream>
#include <cctype>
#include <cstring>
#include <cstdio>
#include <vector>
#include <map>

#include "lua488.hpp"

#include "Light.hpp"
#include "Mesh.hpp"
#include "FractalMountain.hpp"
#include "GeometryNode.hpp"
#include "JointNode.hpp"
#include "Primitive.hpp"
#include "Material.hpp"
#include "PhongMaterial.hpp"
#include "A5.hpp"
#include "LTree.hpp"

typedef std::map<std::string, Mesh *> MeshMap;
static MeshMap mesh_map;

// Uncomment the following line to enable debugging messages
// #define GRLUA_ENABLE_DEBUG

#ifdef GRLUA_ENABLE_DEBUG
#define GRLUA_DEBUG(x)               \
    do                               \
    {                                \
        std::cerr << x << std::endl; \
    } while (0)
#define GRLUA_DEBUG_CALL                        \
    do                                          \
    {                                           \
        std::cerr << __FUNCTION__ << std::endl; \
    } while (0)
#else
#define GRLUA_DEBUG(x) \
    do                 \
    {                  \
    } while (0)
#define GRLUA_DEBUG_CALL \
    do                   \
    {                    \
    } while (0)
#endif

// You may wonder, for the following types, why we use special "_ud"
// types instead of, for example, just allocating SceneNodes directly
// from lua. Part of the answer is that Lua is a C api. It doesn't
// call any constructors or destructors for you, so it's easier if we
// let it just allocate a pointer for the node, and handle
// allocation/deallocation of the node itself. Another (perhaps more
// important) reason is that we will want SceneNodes to stick around
// even after lua is done with them, after all, we want to pass them
// back to the program. If we let Lua allocate SceneNodes directly,
// we'd lose them all when we are done parsing the script. This way,
// we can easily keep around the data, all we lose is the extra
// pointers to it.

// The "userdata" type for a node. Objects of this type will be
// allocated by Lua to represent nodes.
struct gr_node_ud
{
    SceneNode *node;
};

// The "userdata" type for a material. Objects of this type will be
// allocated by Lua to represent materials.
struct gr_material_ud
{
    Material *material;
};

// The "userdata" type for a light. Objects of this type will be
// allocated by Lua to represent lights.
struct gr_light_ud
{
    Light *light;
};

// Useful function to retrieve and check an n-tuple of numbers.
template <typename T>
void get_tuple(lua_State *L, int arg, T *data, int n)
{
    luaL_checktype(L, arg, LUA_TTABLE);
    luaL_argcheck(L, lua_rawlen(L, arg) == n, arg, "N-tuple expected");
    for (int i = 1; i <= n; i++)
    {
        lua_rawgeti(L, arg, i);
        data[i - 1] = luaL_checknumber(L, -1);
        lua_pop(L, 1);
    }
}

// Create a Node
extern "C" int gr_node_cmd(lua_State *L)
{
    GRLUA_DEBUG_CALL;

    gr_node_ud *data = (gr_node_ud *)lua_newuserdata(L, sizeof(gr_node_ud));
    data->node = 0;

    const char *name = luaL_checkstring(L, 1);
    data->node = new SceneNode(name);

    luaL_getmetatable(L, "gr.node");
    lua_setmetatable(L, -2);

    return 1;
}

// Create a Joint node
extern "C" int gr_joint_cmd(lua_State *L)
{
    GRLUA_DEBUG_CALL;

    gr_node_ud *data = (gr_node_ud *)lua_newuserdata(L, sizeof(gr_node_ud));
    data->node = 0;

    const char *name = luaL_checkstring(L, 1);
    JointNode *node = new JointNode(name);

    double x[3], y[3];
    get_tuple(L, 2, x, 3);
    get_tuple(L, 3, y, 3);

    node->set_joint_x(x[0], x[1], x[2]);
    node->set_joint_y(y[0], y[1], y[2]);

    data->node = node;

    luaL_getmetatable(L, "gr.node");
    lua_setmetatable(L, -2);

    return 1;
}

// Create a Sphere node
extern "C" int gr_sphere_cmd(lua_State *L)
{
    GRLUA_DEBUG_CALL;

    gr_node_ud *data = (gr_node_ud *)lua_newuserdata(L, sizeof(gr_node_ud));
    data->node = 0;

    const char *name = luaL_checkstring(L, 1);
    data->node = new GeometryNode(name, new Sphere());

    luaL_getmetatable(L, "gr.node");
    lua_setmetatable(L, -2);

    return 1;
}

// Create a cube node
extern "C" int gr_cube_cmd(lua_State *L)
{
    GRLUA_DEBUG_CALL;

    gr_node_ud *data = (gr_node_ud *)lua_newuserdata(L, sizeof(gr_node_ud));
    data->node = 0;

    const char *name = luaL_checkstring(L, 1);
    data->node = new GeometryNode(name, new Cube());

    luaL_getmetatable(L, "gr.node");
    lua_setmetatable(L, -2);

    return 1;
}

// Create a cube node
extern "C" int gr_cylinder_cmd(lua_State *L)
{
    GRLUA_DEBUG_CALL;

    gr_node_ud *data = (gr_node_ud *)lua_newuserdata(L, sizeof(gr_node_ud));
    data->node = 0;

    const char *name = luaL_checkstring(L, 1);

    int args = lua_gettop(L);

    if (args > 2)
    {
        double radius = luaL_checknumber(L, 2);
        double height = luaL_checknumber(L, 3);
        data->node = new GeometryNode(name, new Cylinder(radius, height));
    }
    else
    {
        data->node = new GeometryNode(name, new Cylinder());
    }

    luaL_getmetatable(L, "gr.node");
    lua_setmetatable(L, -2);

    return 1;
}

// Create a non-hierarchical Sphere node
extern "C" int gr_nh_sphere_cmd(lua_State *L)
{
    GRLUA_DEBUG_CALL;

    gr_node_ud *data = (gr_node_ud *)lua_newuserdata(L, sizeof(gr_node_ud));
    data->node = 0;

    const char *name = luaL_checkstring(L, 1);

    glm::dvec3 pos;
    get_tuple(L, 2, &pos[0], 3);

    double radius = luaL_checknumber(L, 3);

    data->node = new GeometryNode(name, new NonhierSphere(pos, radius));

    luaL_getmetatable(L, "gr.node");
    lua_setmetatable(L, -2);

    return 1;
}

// Create a non-hierarchical Box node
extern "C" int gr_nh_box_cmd(lua_State *L)
{
    GRLUA_DEBUG_CALL;

    gr_node_ud *data = (gr_node_ud *)lua_newuserdata(L, sizeof(gr_node_ud));
    data->node = 0;

    const char *name = luaL_checkstring(L, 1);

    glm::dvec3 pos;
    get_tuple(L, 2, &pos[0], 3);

    double size = luaL_checknumber(L, 3);

    data->node = new GeometryNode(name, new NonhierBox(pos, size));

    luaL_getmetatable(L, "gr.node");
    lua_setmetatable(L, -2);

    return 1;
}

// Create a polygonal Mesh node
extern "C" int gr_mesh_cmd(lua_State *L)
{
    GRLUA_DEBUG_CALL;

    gr_node_ud *data = (gr_node_ud *)lua_newuserdata(L, sizeof(gr_node_ud));
    data->node = 0;

    const char *name = luaL_checkstring(L, 1);
    const char *obj_fname = luaL_checkstring(L, 2);

    std::string sfname(obj_fname);

    // Use a dictionary structure to make sure every mesh is loaded
    // at most once.
    auto i = mesh_map.find(sfname);
    Mesh *mesh = nullptr;

    if (i == mesh_map.end())
    {
        mesh = new Mesh(obj_fname);
        mesh_map[sfname] = mesh;
    }
    else
    {
        mesh = i->second;
    }

    data->node = new GeometryNode(name, mesh);

    luaL_getmetatable(L, "gr.node");
    lua_setmetatable(L, -2);

    return 1;
}

extern "C" int gr_mountain_cmd(lua_State *L)
{
    GRLUA_DEBUG_CALL;

    gr_node_ud *data = (gr_node_ud *)lua_newuserdata(L, sizeof(gr_node_ud));
    data->node = 0;

    const char *name = luaL_checkstring(L, 1);

    int grid_size = luaL_checknumber(L, 2);

    double roughness = luaL_checknumber(L, 3);

    FractalMountain *mesh = new FractalMountain(grid_size, roughness);

    data->node = new GeometryNode(name, mesh);

    luaL_getmetatable(L, "gr.node");
    lua_setmetatable(L, -2);

    return 1;
}

extern "C" int gr_ltree_cmd(lua_State *L)
{
    GRLUA_DEBUG_CALL;

    gr_node_ud *data = (gr_node_ud *)lua_newuserdata(L, sizeof(gr_node_ud));
    data->node = 0;

    const char *name = luaL_checkstring(L, 1);

    luaL_checktype(L, 2, LUA_TTABLE);
    int rule_count = int(lua_rawlen(L, 2));

    luaL_argcheck(L, rule_count >= 1, 2, "Table of rules expected");
    std::map<char, std::string> rules;
    for (int i = 1; i <= rule_count; i++)
    {
        lua_rawgeti(L, 2, i);
        luaL_checktype(L, -1, LUA_TTABLE);
        luaL_argcheck(L, lua_rawlen(L, -1) == 2, 2, "Rule must be a 2-tuple");
        lua_rawgeti(L, -1, 1);
        luaL_argcheck(L, lua_isstring(L, -1), 2, "Rule must be a 2-tuple of strings");
        const char *key = lua_tostring(L, -1);
        lua_pop(L, 1);
        lua_rawgeti(L, -1, 2);
        luaL_argcheck(L, lua_isstring(L, -1), 2, "Rule must be a 2-tuple of strings");
        const char *value = lua_tostring(L, -1);
        lua_pop(L, 1);
        rules[key[0]] = value;
        lua_pop(L, 1);
    }

    int depth = luaL_checknumber(L, 3);

    double angle = luaL_checknumber(L, 4);

    double height = luaL_checknumber(L, 5);

    double radius = luaL_checknumber(L, 6);

    LTree ltree = LTree("F", rules, depth, angle, height, radius);

    data->node = ltree.render_tree();

    luaL_getmetatable(L, "gr.node");
    lua_setmetatable(L, -2);

    return 1;
}

// Make a Point light
extern "C" int gr_light_cmd(lua_State *L)
{
    GRLUA_DEBUG_CALL;

    gr_light_ud *data = (gr_light_ud *)lua_newuserdata(L, sizeof(gr_light_ud));
    data->light = 0;

    Light l;

    double col[3];
    get_tuple(L, 1, &l.position[0], 3);
    get_tuple(L, 2, col, 3);
    get_tuple(L, 3, l.falloff, 3);

    l.colour = glm::dvec3(col[0], col[1], col[2]);

    data->light = new Light(l);

    luaL_newmetatable(L, "gr.light");
    lua_setmetatable(L, -2);

    return 1;
}

// Render a scene
extern "C" int gr_render_cmd(lua_State *L)
{
    GRLUA_DEBUG_CALL;

    gr_node_ud *root = (gr_node_ud *)luaL_checkudata(L, 1, "gr.node");
    luaL_argcheck(L, root != 0, 1, "Root node expected");

    const char *filename = luaL_checkstring(L, 2);

    int width = luaL_checknumber(L, 3);
    int height = luaL_checknumber(L, 4);

    glm::dvec3 eye;
    glm::dvec3 view, up;

    get_tuple(L, 5, &eye[0], 3);
    get_tuple(L, 6, &view[0], 3);
    get_tuple(L, 7, &up[0], 3);

    double fov = luaL_checknumber(L, 8);

    double ambient_data[3];
    get_tuple(L, 9, ambient_data, 3);
    glm::dvec3 ambient(ambient_data[0], ambient_data[1], ambient_data[2]);

    luaL_checktype(L, 10, LUA_TTABLE);
    int light_count = int(lua_rawlen(L, 10));

    luaL_argcheck(L, light_count >= 1, 10, "Tuple of lights expected");
    std::list<Light *> lights;
    for (int i = 1; i <= light_count; i++)
    {
        lua_rawgeti(L, 10, i);
        gr_light_ud *ldata = (gr_light_ud *)luaL_checkudata(L, -1, "gr.light");
        luaL_argcheck(L, ldata != 0, 10, "Light expected");

        lights.push_back(ldata->light);
        lua_pop(L, 1);
    }

    int args = lua_gettop(L);
    bool anti_aliasing = false;
    if (args > 10)
    {
        anti_aliasing = lua_toboolean(L, 11);
    }

    Image im(width, height);
    A5_Render(root->node, im, eye, view, up, fov, ambient, lights, anti_aliasing);
    im.savePng(filename);

    return 0;
}

// Create a Material
extern "C" int gr_material_cmd(lua_State *L)
{
    GRLUA_DEBUG_CALL;

    gr_material_ud *data = (gr_material_ud *)lua_newuserdata(L, sizeof(gr_material_ud));
    data->material = 0;

    double kd[3], ks[3], ke[3], absorption[3];
    get_tuple(L, 1, kd, 3);
    get_tuple(L, 2, ks, 3);
    ke[0] = 0.0;
    ke[1] = 0.0;
    ke[2] = 0.0;

    absorption[0] = 0.0;
    absorption[1] = 0.0;
    absorption[2] = 0.0;

    double shininess = luaL_checknumber(L, 3);
    float ior = 0.f;
    double transparency = 0.0;
    double reflectivity = 0.0;
    double transparency_glossiness = 0.0;
    double reflectivity_glossiness = 0.0;

    int arg_count = lua_gettop(L);
    if (arg_count >= 7)
    {
        get_tuple(L, 4, ke, 3);
        ior = luaL_checknumber(L, 5);
        transparency = luaL_checknumber(L, 6);
        reflectivity = luaL_checknumber(L, 7);
    }

    if (arg_count > 8)
    {
        get_tuple(L, 8, absorption, 3);
    }

    if (arg_count > 10)
    {
        transparency_glossiness = luaL_checknumber(L, 9);
        reflectivity_glossiness = luaL_checknumber(L, 10);
    }

    std::string texture_path = "";
    if (arg_count > 11)
    {
        texture_path = luaL_checkstring(L, 11);
    }

    std::string normal_map_path = "";
    if (arg_count > 12)
    {
        normal_map_path = luaL_checkstring(L, 12);
    }

    data->material = new PhongMaterial(glm::dvec3(kd[0], kd[1], kd[2]),
                                       glm::dvec3(ks[0], ks[1], ks[2]),
                                       shininess,
                                       glm::dvec3(ke[0], ke[1], ke[2]),
                                       ior, transparency, reflectivity,
                                       glm::dvec3(absorption[0], absorption[1], absorption[2]),
                                       transparency_glossiness, reflectivity_glossiness,
                                       texture_path, normal_map_path);

    luaL_newmetatable(L, "gr.material");
    lua_setmetatable(L, -2);

    return 1;
}

// Add a Child to a node
extern "C" int gr_node_add_child_cmd(lua_State *L)
{
    GRLUA_DEBUG_CALL;

    gr_node_ud *selfdata = (gr_node_ud *)luaL_checkudata(L, 1, "gr.node");
    luaL_argcheck(L, selfdata != 0, 1, "Node expected");

    SceneNode *self = selfdata->node;

    gr_node_ud *childdata = (gr_node_ud *)luaL_checkudata(L, 2, "gr.node");
    luaL_argcheck(L, childdata != 0, 2, "Node expected");

    SceneNode *child = childdata->node;

    self->add_child(child);

    return 0;
}

// Set a node's Material
extern "C" int gr_node_set_material_cmd(lua_State *L)
{
    GRLUA_DEBUG_CALL;

    gr_node_ud *selfdata = (gr_node_ud *)luaL_checkudata(L, 1, "gr.node");
    luaL_argcheck(L, selfdata != 0, 1, "Node expected");

    GeometryNode *self = dynamic_cast<GeometryNode *>(selfdata->node);

    luaL_argcheck(L, self != 0, 1, "Geometry node expected");

    gr_material_ud *matdata = (gr_material_ud *)luaL_checkudata(L, 2, "gr.material");
    luaL_argcheck(L, matdata != 0, 2, "Material expected");

    Material *material = matdata->material;

    self->setMaterial(material);

    return 0;
}

extern "C" int gr_node_set_texture_tile_cmd(lua_State *L)
{
    GRLUA_DEBUG_CALL;

    gr_node_ud *selfdata = (gr_node_ud *)luaL_checkudata(L, 1, "gr.node");
    luaL_argcheck(L, selfdata != 0, 1, "Node expected");

    GeometryNode *self = dynamic_cast<GeometryNode *>(selfdata->node);

    luaL_argcheck(L, self != 0, 1, "Geometry node expected");

    double tiling[2];

    for (int i = 0; i < 2; i++)
    {
        tiling[i] = luaL_checknumber(L, i + 2);
    }

    self->setTextureTiling(glm::dvec2(tiling[0], tiling[1]));

    return 0;
}

// Add a Scaling transformation to a node.
extern "C" int gr_node_scale_cmd(lua_State *L)
{
    GRLUA_DEBUG_CALL;

    gr_node_ud *selfdata = (gr_node_ud *)luaL_checkudata(L, 1, "gr.node");
    luaL_argcheck(L, selfdata != 0, 1, "Node expected");

    SceneNode *self = selfdata->node;

    double values[3];

    for (int i = 0; i < 3; i++)
    {
        values[i] = luaL_checknumber(L, i + 2);
    }

    self->scale(glm::dvec3(values[0], values[1], values[2]));

    return 0;
}

// Add a Translation to a node.
extern "C" int gr_node_translate_cmd(lua_State *L)
{
    GRLUA_DEBUG_CALL;

    gr_node_ud *selfdata = (gr_node_ud *)luaL_checkudata(L, 1, "gr.node");
    luaL_argcheck(L, selfdata != 0, 1, "Node expected");

    SceneNode *self = selfdata->node;

    double values[3];

    for (int i = 0; i < 3; i++)
    {
        values[i] = luaL_checknumber(L, i + 2);
    }

    self->translate(glm::dvec3(values[0], values[1], values[2]));

    return 0;
}

// Rotate a node.
extern "C" int gr_node_rotate_cmd(lua_State *L)
{
    GRLUA_DEBUG_CALL;

    gr_node_ud *selfdata = (gr_node_ud *)luaL_checkudata(L, 1, "gr.node");
    luaL_argcheck(L, selfdata != 0, 1, "Node expected");

    SceneNode *self = selfdata->node;

    const char *axis_string = luaL_checkstring(L, 2);

    luaL_argcheck(L, axis_string && std::strlen(axis_string) == 1, 2, "Single character expected");
    char axis = std::tolower(axis_string[0]);

    luaL_argcheck(L, axis >= 'x' && axis <= 'z', 2, "Axis must be x, y or z");

    double angle = luaL_checknumber(L, 3);

    self->rotate(axis, angle);

    return 0;
}

// Garbage collection function for lua.
extern "C" int gr_node_gc_cmd(lua_State *L)
{
    GRLUA_DEBUG_CALL;

    gr_node_ud *data = (gr_node_ud *)luaL_checkudata(L, 1, "gr.node");
    luaL_argcheck(L, data != 0, 1, "Node expected");

    // Note that we don't delete the node here. This is because we still
    // want the scene to be around when we close the lua interpreter,
    // but at that point everything will be garbage collected.
    //
    // If data->node happened to be a reference-counting pointer, this
    // will in fact just decrease lua's reference to it, so it's not a
    // bad thing to include this line.
    data->node = 0;

    return 0;
}

// This is where all the "global" functions in our library are
// declared.
// If you want to add a new non-member function, add it HERE.
static const luaL_Reg grlib_functions[] = {
    {"node", gr_node_cmd},
    {"sphere", gr_sphere_cmd},
    {"joint", gr_joint_cmd},
    {"material", gr_material_cmd},
    // New for assignment 4
    {"cube", gr_cube_cmd},
    {"cylinder", gr_cylinder_cmd},
    {"nh_sphere", gr_nh_sphere_cmd},
    {"nh_box", gr_nh_box_cmd},
    {"mesh", gr_mesh_cmd},
    {"mountain", gr_mountain_cmd},
    {"ltree", gr_ltree_cmd},
    {"light", gr_light_cmd},
    {"render", gr_render_cmd},
    {0, 0}};

// This is where all the member functions for "gr.node" objects are
// declared. Since all the other objects (e.g. materials) are so
// simple, we only really need to make member functions for nodes.
//
// If you want to add a new member function for gr.node, add it
// here.
//
// We could have used inheritance in lua to match the inheritance
// between different node types, but it's easier to just give all
// nodes the same Lua type and then do any additional type checking in
// the appropriate member functions (e.g. gr_node_set_material_cmd
// ensures that the node is a GeometryNode, see above).
static const luaL_Reg grlib_node_methods[] = {
    {"__gc", gr_node_gc_cmd},
    {"add_child", gr_node_add_child_cmd},
    {"set_material", gr_node_set_material_cmd},
    {"set_texture_tiling", gr_node_set_texture_tile_cmd},
    {"scale", gr_node_scale_cmd},
    {"rotate", gr_node_rotate_cmd},
    {"translate", gr_node_translate_cmd},
    {"render", gr_render_cmd},
    {0, 0}};

// This function calls the lua interpreter to define the scene and
// raytrace it as appropriate.
bool run_lua(const std::string &filename)
{
    GRLUA_DEBUG("Importing scene from " << filename);

    // Start a lua interpreter
    lua_State *L = luaL_newstate();

    GRLUA_DEBUG("Loading base libraries");

    // Load some base library
    luaL_openlibs(L);

    GRLUA_DEBUG("Setting up our functions");

    // Set up the metatable for gr.node
    luaL_newmetatable(L, "gr.node");
    lua_pushstring(L, "__index");
    lua_pushvalue(L, -2);
    lua_settable(L, -3);

    // Load the gr.node methods
    luaL_setfuncs(L, grlib_node_methods, 0);

    // Load the gr functions
    luaL_setfuncs(L, grlib_functions, 0);
    lua_setglobal(L, "gr");

    GRLUA_DEBUG("Parsing the scene...");
    // Now parse the actual scene
    if (luaL_loadfile(L, filename.c_str()) || lua_pcall(L, 0, 0, 0))
    {
        std::cerr << "Error loading " << filename << ": " << lua_tostring(L, -1) << std::endl;
        return false;
    }
    GRLUA_DEBUG("Closing the interpreter");

    // Close the interpreter, free up any resources not needed
    lua_close(L);

    return true;
}
