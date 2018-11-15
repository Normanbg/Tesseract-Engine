#include "Application.h"
#include "ModuleResource.h"
#include "ModuleSceneLoader.h"
#include "ModuleTextures.h"
#include "ModuleFileSystem.h"

#include "Resource.h"
#include "ResourceMesh.h"
#include "ResourceScene.h"
#include "ResourceTexture.h"

ModuleResource::ModuleResource(bool start_enabled):Module(start_enabled)
{
}


ModuleResource::~ModuleResource()
{
}

bool ModuleResource::Start()
{
	bool ret = true;
	return true;
}

update_status ModuleResource::Update(float dt)
{
	return UPDATE_CONTINUE;
}

bool ModuleResource::CleanUp()
{
	bool ret = true;

	resources.clear();

	return true;
}

uint ModuleResource::Find(const char * file) const
{
	return uint();
}

uint ModuleResource::ImportFile(const char* file, ResType type)
{
	uint ret = 0;
	std::string written_file;
	bool loaded = false;

	bool newMeta = false;
	JSON_File* meta = getMeta(file);
	if (meta == nullptr)
	{	
		meta = createMeta(file, type);
		newMeta = true;
	}
	uint UID = meta->getValue("meta")->getUint("UID");

	switch (type)
	{
	case R_TEXTURE:
		loaded=App->textures->importTexture(file,written_file, meta->getValue("meta")->getValue("ImportSettings"));
		break;
	case R_SCENE:
		loaded=App->scene_loader->importFBXScene(file,written_file, meta->getValue("meta")->getValue("ImportSettings"));
		break;
	}

	if (loaded)
	{
		Resource* newRes = AddResource(type, UID);
		newRes->file = file;
		newRes->exported_file = written_file;
		ret = newRes->UID;
		LOG("Source Created");
	}
	if (newMeta)
		meta->Write();

	App->JSON_manager->closeFile(meta);	
	return ret;
}

uint ModuleResource::GenerateNewUID()
{
	return ++last_UID;
}

Resource * ModuleResource::GetResource(uint uid)
{
	std::map <uint, Resource* >::iterator it = resources.find(uid);
	
	if (resources.find(uid) != resources.end())
	{
		return resources[uid];
	}

	return nullptr;
}

Resource * ModuleResource::AddResource(ResType type, uint forced_uid)
{
	Resource* ret =nullptr;

	//TEMP
	forced_uid = GENERATE_UID();

	switch (type)
	{
	case NO_TYPE:
		ret = new Resource(forced_uid,NO_TYPE);
		break;
	case R_MESH:
		ret = (Resource*)new ResourceMesh(forced_uid, R_MESH);
		break;
	case R_TEXTURE:
		ret = (Resource*)new ResourceTexture(forced_uid, R_TEXTURE);
		break;
	case R_SCENE:
		ret = (Resource*)new ResourceScene(forced_uid, R_SCENE);
		break;
	}
	
	resources.insert(std::pair<uint,Resource*>(forced_uid,ret));

	return ret;
}

uint ModuleResource::GetResourceByFile(const char* file)
{
	uint ret = 0;

	for (std::map<uint, Resource*>::iterator it_rs = resources.begin(); it_rs != resources.end(); it_rs++)
	{
		if ((*it_rs).second->file == file)
		{
			ret = (*it_rs).second->UID;
			(*it_rs).second->LoadInMemory();
		}
	}

	return ret;
}

JSON_File* ModuleResource::getMeta(const char* path) const
{
	std::string metaPath = path;
	metaPath += META_EXTENSION;
	JSON_File* ret = App->JSON_manager->openReadFile(metaPath.c_str());

	return ret;
}

JSON_File* ModuleResource::createMeta(const char* path, ResType type) const
{
	std::string metaPath = path;
	metaPath += META_EXTENSION;
	JSON_File* ret = App->JSON_manager->openWriteFile(metaPath.c_str());

	JSON_Value* meta = ret->createValue();
	meta->addUint("UID", GENERATE_UID());
	meta->addInt("last_change", App->fileSystem->getLastTimeChanged(path));
	
	JSON_Value* importSettings = meta->createValue();
	switch (type)
	{
	case R_MESH:
		ResourceMesh::setImportDefaults(*importSettings);
		break;
	case R_TEXTURE:
		ResourceTexture::setImportDefaults(*importSettings);
		break;
	case R_SCENE:
		ResourceScene::setImportDefaults(*importSettings);
		break;
	}
	
	meta->addValue("ImportSettings", importSettings);
	ret->addValue("meta", meta);

	return ret;
}

bool ModuleResource::updateMetaLastChange(const char* path)
{
	bool ret = false;
	std::string metaPath = path;
	metaPath += META_EXTENSION;
	JSON_File* readFile = App->JSON_manager->openReadFile(metaPath.c_str());
	if (readFile != nullptr)
	{
		JSON_File* writeFile = App->JSON_manager->openWriteFile(metaPath.c_str());
		JSON_Value* meta = readFile->getValue("meta");
		meta->setUint("last_change", App->fileSystem->getLastTimeChanged(path));
		writeFile->addValue("meta", meta);

		writeFile->Write();
		App->JSON_manager->closeFile(writeFile);
		ret = true;
	}
	App->JSON_manager->closeFile(readFile);
	return ret;
}
