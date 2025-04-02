#pragma once
#include "Systems/System.h"

#include <Spatial/ChunkPosition.h>
#include <Spatial/ChunkSource.h>
#include "Utility/stdHashing.h"
#include <Utility/ChunkSerializer.h>

namespace drft::events
{
	struct ChunkSourceTransferRequestEvent;
}

namespace drft::system
{
	struct CameraHandle;
	
	class ChunkManager : public System
	{
	private:
		enum class State
		{
			NoSource,
			Transferring,
			SourceReady
		};
	public:
		using System::System;

		virtual void init() override;
		virtual void update() override;
		virtual void shutdown() override;

	private:
		void onNoSource();
		void onTransfer();
		void onUpdateSource();

		void setState(State newState);
		void onStateChange(State newState);

		void onRequestChunkSourceChangeEvent(events::ChunkSourceTransferRequestEvent& ev);

	private:
		struct PendingTransfer
		{
			entt::id_type oldSourceId;
			entt::id_type newSourceId;
		};

	private:
		using SourcePtr = std::unique_ptr<spatial::ChunkSource>;
		ChunkSerializer _serializer;
		State _state = State::NoSource;
		SourcePtr _activeSource;
		std::optional<PendingTransfer> _pendingTransfer = std::nullopt;
	};
}



