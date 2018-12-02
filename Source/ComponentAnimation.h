#ifndef __COMPONENTANIMATION_H__
#define __COMPONENTANIMATION_H__

#include "Component.h"

class GameObject;

class ComponentAnimation : public Component
{
public:

	ComponentAnimation(GameObject* parent, componentType type);
	~ComponentAnimation();

	bool Update();
	void DrawInfo();

	void Save(JSON_Value* component) const;
	void Load(JSON_Value* component);


};

#endif // !__COMPONENTANIMATION_H__