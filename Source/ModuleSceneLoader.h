#ifndef __MODULESCENELOADER_H__
#define __MODULESCENELOADER_H__

#include "Module.h"
#include <vector>
#include <map>

class GameObject;
class ResourceMesh;
class ResourceAnimation;
class ResourceMaterial;
class ResourceBone;
class ComponentMesh;

struct aiScene;
struct aiNode; 
struct aiMesh;

struct JSON_Value;

class ModuleSceneLoader : public Module
{
public:
	ModuleSceneLoader(bool start_enabled = true);
	~ModuleSceneLoader();

	bool Init(JSON_File* document);

	bool CleanUp();

	bool importScene(const char* path, uint UID, std::vector<uint>& meshUIDs, std::vector<uint>& animationUIDs, std::vector<uint>& bonesUIDs, std::string& newPath,JSON_Value* meta, bool newMeta = false);
	GameObject* loadGameObject(const aiScene* scene, aiNode* node, std::vector<ResourceMesh*> meshes, std::vector<ResourceMaterial*> textures, GameObject* fakeScene, std::map<uint, ComponentMesh*>& meshlinker);

	std::vector<ResourceMesh*> importMeshes(const char* path, const aiScene* scene, std::vector<uint>& meshUIDs, std::vector<uint>& bonesUIDs, std::vector<ResourceBone*>& rBones, JSON_Value* meta, bool newMeta = false);
	std::vector<ResourceAnimation*> importAnimations(const char* path, const aiScene* scene, std::vector<uint>& animationUIDs, JSON_Value* meta, bool newMeta = false);
	std::vector<uint> importBonesFromMesh(const char* path, const aiMesh* mesh, std::vector<uint>& bonesUIDs, std::vector<ResourceBone*>& rBones, std::map<std::string*, uint>& boneUIDs, std::vector<std::string*> bonesNames, uint meshUID, JSON_Value* meta, bool newMeta = false);
	std::vector<ResourceMaterial*> importMaterials(const char* path, const aiScene* scene);
	ResourceMaterial* importColor(const char* path, const aiScene* scene, int index);

	bool saveSceneFile(const char* scene_name, GameObject* fakeRoot = nullptr);
	bool saveVirtualScene();

	bool loadSceneFile(const char* scene_name, bool isFBX = false);
	bool loadVirtualScene();

	bool saveScene(JSON_File* scene, GameObject* rootObject);
	bool loadScene(JSON_File* scene);

public:

	std::string next_scene_path = "";

private:

	int root_counter=0;	
};

#endif // !__MODULESCENELOADER_H__