#pragma once
#include "Systems/System.h"

#include <Spatial/ChunkSource.h>
#include <Utility/ChunkSerializer.h>
#include <ProcGen/ChunkGenerator.h>
#include <EnTT/entt.h>
#include <memory>
#include <optional>

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
			FirstUpdate,
			NoSource,
			Transferring,
			SourceReady
		};
		using SourcePtr = std::unique_ptr<spatial::ChunkSource>;
	public:
		using System::System;

		virtual void init() override;
		virtual void start() override;
		virtual void update() override;
		virtual void shutdown() override;

	private:
		void onFirstUpdate();
		void onNoSource();
		void onTransfer();
		void onUpdateSource();

		void setState(State newState);
		void onStateChange(State newState);

		void notifyTransferFailed(entt::id_type sourceId);

		void onChunkSourceTransferRequestEvent(events::ChunkSourceTransferRequestEvent& ev);
		void onConstructChunkSourceTracker(entt::registry& registry, entt::entity entity);
		void onUpdateCameraTarget(entt::registry& registry, entt::entity entity);

		SourcePtr tryCreateNewChunkSource(entt::id_type sourceId);
		bool doesChunkSourceExist(entt::id_type sourceId) const;

	private:
		struct PendingTransfer
		{
			entt::id_type oldSourceId;
			entt::id_type newSourceId;
			TilePosition position;
		};

	private:
		ChunkSerializer _serializer;
		ChunkGenerator _generator;

		State _state = State::FirstUpdate;
		SourcePtr _activeSource;
		entt::entity _chunkSourceTracker = entt::null;
		std::optional<PendingTransfer> _pendingTransfer = std::nullopt;
		bool _hasInitializedNewSource = false;
	};
}



