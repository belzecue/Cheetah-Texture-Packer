#ifndef COMMANDLIST_H
#define COMMANDLIST_H
#include "Support/counted_string.h"
#include "Support/counted_ptr.hpp"
#include "enums.hpp"
#include "commandinterface.hpp"
#include "Sprite/document.h"
#include "Sprite/object.h"
#include <glm/vec4.hpp>
#include <glm/vec2.hpp>

struct Document;
struct Texture;
struct Animation;
struct Attachment;
struct Object;


class AnimationCommand : public CommandInterface
{
typedef AnimationCommand self_type;
public:
	//insert/delete
	AnimationCommand(Document * doc, int object, int animation, Animation * insert = nullptr);
	virtual ~AnimationCommand() = default;

	void RollForward();
	void RollBack();

private:
	void InsertAnimation();
	void RemoveAnimation();
	void UpdateAnimation(Animation const&);

	counted_ptr<Object>    object;
	counted_ptr<Animation> animation;
	Animation            * next{&next_state};
	Animation            * prev{&prev_state};
	Animation              prev_state;
	Animation              next_state;
};

class ObjectCommand : public CommandInterface
{
public:
	ObjectCommand(Document * doc, int object, std::string insert);
	virtual ~ObjectCommand() = default;

	void RollForward();
	void RollBack();


private:
	Document          * doc{};
	counted_ptr<Object> object;
	counted_string      old_name;
	counted_string      new_name;
	uint32_t            old_offset{};
	uint32_t            new_offset{};
};


class AttachmentCommand : public CommandInterface
{
typedef AttachmentCommand self_type;
public:
	//insert/delete
	AttachmentCommand(Document * doc, int object, int attachment, std::string insert);
	virtual ~AttachmentCommand() = default;

	void RollForward();
	void RollBack();

private:
	counted_ptr<Object>       object;
	counted_string            attachment;
	counted_string            new_attachment;
	std::vector<glm::i16vec2> att_points;
};

template<typename T>
class UpdateMaterialCommand : public CommandInterface
{
public:
	UpdateMaterialCommand(Document * doc, int object, intptr_t offset, T value) :
		document(doc),
		object(doc->objects[object]),
		offset(offset),
		originalValue(*GetValue()),
		finalValue(value)
	{
		RollForward();
	}
	virtual ~UpdateMaterialCommand() = default;

	T * GetValue() { return (T*)(((uint8_t*)&object->material) + offset); }

	void RollForward() 	{ *GetValue() = finalValue;    Update(); }
	void RollBack()     { *GetValue() = originalValue; Update(); }

	void Update() {}

private:
	Document * document;
	counted_ptr<Object> object;
	intptr_t            offset;
	T                   originalValue;
	T                   finalValue;
};

template<>
inline void UpdateMaterialCommand<counted_ptr<Image>>::Update()
{
	object->UpdateImages(document);
}


#endif // COMMANDLIST_H
