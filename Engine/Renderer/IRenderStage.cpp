#include "IRenderStage.h"
#include <RDirectX.h>
#include <RenderTarget.h>
#include <LightGroup.h>
#include <Camera.h>

void IRenderStage::AllCall()
{
	RenderOrder memo;

	for (RenderOrder& order : mOrders) {
		//直前実行処理
		if(order.preCommand) order.preCommand();

		//自動設定項目の設定
		if (order.primitiveTopology == D3D_PRIMITIVE_TOPOLOGY_UNDEFINED) {
			order.primitiveTopology = mDefParamater.primitiveTopology;
		}
		if (order.renderTargets.empty()) {
			order.renderTargets = mDefParamater.renderTargets;
		}
		if (order.viewports.empty()) {
			order.viewports = mDefParamater.viewports;
		}
		if (order.scissorRects.empty()) {
			order.scissorRects = mDefParamater.scissorRects;
		}
		if (order.mRootSignature == nullptr) {
			order.mRootSignature = mDefParamater.mRootSignature;
		}
		if (order.pipelineState == nullptr) {
			order.pipelineState = mDefParamater.pipelineState;
		}

		if (memo.primitiveTopology != order.primitiveTopology) {
			memo.primitiveTopology = order.primitiveTopology;
			RDirectX::GetCommandList()->IASetPrimitiveTopology(order.primitiveTopology);
		}

		if (memo.renderTargets != order.renderTargets) {
			memo.renderTargets = order.renderTargets;
			if (order.renderTargets.empty()) {
				RenderTarget::SetToBackBuffer();
			}
			else {
				RenderTarget::SetToTexture(order.renderTargets);
			}
		}

		if (memo.viewports != order.viewports) {
			memo.viewports = order.viewports;
			std::vector<D3D12_VIEWPORT> vec;
			for (Viewport& vp : order.viewports) {
				D3D12_VIEWPORT viewport{};
				viewport.Width = vp.width;
				viewport.Height = vp.height;
				viewport.TopLeftX = vp.topleftX;
				viewport.TopLeftY = vp.topleftY;
				viewport.MinDepth = vp.minDepth;
				viewport.MaxDepth = vp.maxDepth;
				vec.push_back(viewport);
			}
			RDirectX::GetCommandList()->RSSetViewports(static_cast<uint32_t>(order.viewports.size()), &vec[0]);
		}

		if (memo.scissorRects != order.scissorRects) {
			memo.scissorRects = order.scissorRects;
			std::vector<D3D12_RECT> vec;
			for (RRect& r : order.scissorRects) {
				D3D12_RECT rect{};
				rect.left = r.left;
				rect.right = r.right;
				rect.top = r.top;
				rect.bottom = r.bottom;
				vec.push_back(rect);
			}
			RDirectX::GetCommandList()->RSSetScissorRects(static_cast<uint32_t>(order.scissorRects.size()), &vec[0]);
		}

		if (memo.mRootSignature != order.mRootSignature) {
			memo.mRootSignature = order.mRootSignature;
			RDirectX::GetCommandList()->SetGraphicsRootSignature(order.mRootSignature);
		}

		if (memo.pipelineState != order.pipelineState) {
			memo.pipelineState = order.pipelineState;
			RDirectX::GetCommandList()->SetPipelineState(order.pipelineState);
		}

		//頂点nullチェック
		if (!order.vertBuff.IsValid() && order.vertView == nullptr) {
#ifdef _DEBUG
			OutputDebugStringA(Util::StringFormat("RKEngine WARNING: IRenderStage::AllCall() : Vertex is null. Draw skip.\n").c_str());
#endif
			continue;
		}

		D3D12_VERTEX_BUFFER_VIEW useVertView{};
		if (order.vertBuff.IsValid()) {
			useVertView = order.vertBuff.GetVertView();
		}
		else {
			useVertView = *order.vertView;
		}

		if (order.instanceVertBuff.IsValid() || order.instanceVertView != nullptr) {
			D3D12_VERTEX_BUFFER_VIEW useInstanceView{};
			if (order.instanceVertBuff.IsValid()) {
				useInstanceView = order.instanceVertBuff.GetVertView();
			}
			else {
				useInstanceView = *order.instanceVertView;
			}

			const D3D12_VERTEX_BUFFER_VIEW buf[2] = {
				useVertView,
				useInstanceView
			};
			RDirectX::GetCommandList()->IASetVertexBuffers(0, 2, buf);
		}
		else {
			RDirectX::GetCommandList()->IASetVertexBuffers(0, 1, &useVertView);
		}

		if (order.indexBuff.IsValid() || order.indexView != nullptr) {
			D3D12_INDEX_BUFFER_VIEW useIndexView{};
			if (order.indexBuff.IsValid()) {
				useIndexView = order.indexBuff.GetIndexView();
			}
			else {
				useIndexView = *order.indexView;
			}
			RDirectX::GetCommandList()->IASetIndexBuffer(&useIndexView);
		}

		int32_t rootIndex = 0;
		for (RootData& data : order.rootData) {
			if (data.type == RootDataType::DESCRIPTOR_TABLE) {
				RDirectX::GetCommandList()->SetGraphicsRootDescriptorTable(rootIndex, data.descriptor);
			}
			else if (data.type == RootDataType::CBV) {
				RDirectX::GetCommandList()->SetGraphicsRootConstantBufferView(rootIndex, data.address);
			}
			else if (data.type == RootDataType::SRBUFFER_CBV) {
				RDirectX::GetCommandList()->SetGraphicsRootConstantBufferView(rootIndex, data.addressSRBuff.GetGPUVirtualAddress());
			}
			else if (data.type == RootDataType::CAMERA) {
				RDirectX::GetCommandList()->SetGraphicsRootConstantBufferView(rootIndex, Camera::sNowCamera->mBuff.mBuff.GetGPUVirtualAddress());
			}
			else if (data.type == RootDataType::LIGHT) {
				RDirectX::GetCommandList()->SetGraphicsRootConstantBufferView(rootIndex, LightGroup::sNowLight->mBuffer.mConstBuff->GetGPUVirtualAddress());
			}
			else {
#ifdef _DEBUG
				OutputDebugStringA("RKEngine WARNING: IRenderStage::AllCall() : Undefined RootDataType.\n");
#endif
			}
			rootIndex++;
		}

		if (order.indexBuff.IsValid() || order.indexView != nullptr) {
			RDirectX::GetCommandList()->DrawIndexedInstanced(static_cast<uint32_t>(order.indexCount), order.instanceCount, 0, 0, 0);
		}
		else {
			RDirectX::GetCommandList()->DrawInstanced(static_cast<uint32_t>(order.indexCount), order.instanceCount, 0, 0);
		}

		//直後実行処理
		if (order.postCommand) order.postCommand();
	}
}
