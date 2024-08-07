#pragma once
#include <ProcGen/GenerationLayer/AbstractChunk.h>

template<typename Layer, typename Chunk>
class GenerationChunk : public AbstractChunk
{
public:
	virtual GenerationState generate() override { return GenerationState::Complete; };
	virtual void destroy() override {};
	virtual bool isGenerated() const override;

protected:
	void setIsGenerated(bool value) { _isGenerated = value; }

protected:
	friend class AbstractLayer;
	Layer* _layer;

private:
	bool _isGenerated = false;
};

#include <ProcGen/GenerationLayer/GenerationChunk.inl>

template<typename Layer, typename Chunk>
inline bool GenerationChunk<Layer, Chunk>::isGenerated() const
{
	return _isGenerated;
}
