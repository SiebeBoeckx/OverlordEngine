#include "stdafx.h"
#include "ParticleEmitterComponent.h"
#include "Misc/ParticleMaterial.h"

ParticleMaterial* ParticleEmitterComponent::m_pParticleMaterial{};

ParticleEmitterComponent::ParticleEmitterComponent(const std::wstring& assetFile, const ParticleEmitterSettings& emitterSettings, UINT particleCount):
	m_ParticlesArray(new Particle[particleCount]),
	m_ParticleCount(particleCount), //How big is our particle buffer?
	m_MaxParticles(particleCount), //How many particles to draw (max == particleCount)
	m_AssetFile(assetFile),
	m_EmitterSettings(emitterSettings)
{
	m_enablePostDraw = true; //This enables the PostDraw function for the component
}

ParticleEmitterComponent::~ParticleEmitterComponent()
{
	//TODO_W9(L"Implement Destructor")
	//1. Delete the Particle Pool (m_ParticlesArray) 
	SafeDelete(m_ParticlesArray);

	//2. Release the VertexBuffer (m_pVertexBuffer) 
	SafeRelease(m_pVertexBuffer);
}

void ParticleEmitterComponent::Initialize(const SceneContext& sceneContext)
{
	//TODO_W9(L"Implement Initialize")
	//1. Use the MaterialManager to create an instance of the ParticleMaterial and store it in
	//m_pParticleMaterial.Note that this member is STATIC, so only create an instance if it isn’t
	if (m_pParticleMaterial == nullptr)
	{
		m_pParticleMaterial = MaterialManager::Get()->CreateMaterial<ParticleMaterial>();
	}

	//2. Call CreateVertexBuffer() 
	CreateVertexBuffer(sceneContext);

	//3. Use the ContentManager to load the particle texture and store it in m_pParticleTexture 
	m_pParticleTexture = ContentManager::Load<TextureData>(m_AssetFile);
}

void ParticleEmitterComponent::CreateVertexBuffer(const SceneContext& sceneContext)
{
	//TODO_W9(L"Implement CreateVertexBuffer")
	//1. If m_pVertexBuffer exists, release it 
	if (m_pVertexBuffer)
	{
		SafeRelease(m_pVertexBuffer);
	}

	/*2. Create a dynamic vertexbuffer
		a. Set the usage to Dynamic
		b. Use the ParticleCount and size of a VertexParticle to calculate the ByteWidth
		c. This is a VertexBuffer, select the appropriate BindFlag
		d. We want to edit the buffer at-runtime, so we need CPU write access. Select the
		appropriate CPUAccessFlag
		e. There are no MiscFlags (0)
		f. Create the VertexBuffer (GraphicsDevice >...)
		g. Don’t forget to check for errors! (HANDLE_ERROR)
		 */
	D3D11_BUFFER_DESC bd;
	bd.Usage = D3D11_USAGE_DYNAMIC;
	bd.ByteWidth = sizeof(VertexParticle) * m_ParticleCount;
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	bd.MiscFlags = 0;
	bd.StructureByteStride = sizeof(VertexParticle);

	HRESULT hr{ sceneContext.d3dContext.pDevice->CreateBuffer(&bd, nullptr, &m_pVertexBuffer) };
	if (FAILED(hr))
	{
		HANDLE_ERROR(L"Failed to create vertexbuffer, ParticleEmitterComponent");
	}
}

void ParticleEmitterComponent::Update(const SceneContext& sceneContext)
{
	//TODO_W9(L"Implement Update");
	const float elapsedTime = sceneContext.pGameTime->GetElapsed();
	//In the update, we are going to emit/spawn new particles (if needed) and update the existing 
	//particles.After updating the particles, we’ll add the Vertex data of the(remaining) active ones to our VertexBuffer.

	//1. Create a local variable, called particleInterval of type float.This variable needs to contain the
	//average particle emit threshold.We know the average energy of a particle(maxEnergy vs
	//	minEnergy) and the number of particles; with those values we can calculate the value for
	//particleInterval.
	float particleInterval{ (m_EmitterSettings.maxEnergy + m_EmitterSettings.minEnergy) / m_ParticleCount };

	//2. Increase m_LastParticleInit by the elapsed GameTime.This value keeps track of the last particle
	//spawn.
	m_LastParticleSpawn += elapsedTime;

	//3. Time to validate the particles and add some of them to the VertexBuffer
	//a.Set m_ActiveParticles to zero
	m_ActiveParticles = 0;

	//b.Use DeviceContext::Map to map our vertexbuffer, this method retrieves an element of type
	//D3D11_MAPPED_SUBRESOURCE which contains a pointer to the first element in our
	//vertexbuffer.Between the ‘Map’and ‘Unmap’ method we can update the VertexBuffer
	//data.
		//i.We want to override the current data in the vertexbuffer, so we can discard the
		//previous data(D3D11_MAP_WRITE_DISCARD)
		//ii.There are no mapflags(0)
		//iii.The last parameter is of type D3D11_MAPPED_SUBRESOURCE which contains a
		//pointer(pData) to the first VertexParticle element in the buffer.
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	sceneContext.d3dContext.pDeviceContext->Map(m_pVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	//c.Create an empty pointer of type VertexParticle(ParticleVertex* pBuffer) and cast& assign
	//pData to it.
	VertexParticle* pBuffer = reinterpret_cast<VertexParticle*>(mappedResource.pData);

	//d.The VertexBuffer is mapped, and we have access to write new data to it.
	//e.Iterate the Particle Array(You only want to iterate ‘m_ParticleCount’ number of times)
	for (size_t i{0}; i < m_ParticleCount; i++)
	{
		//f.For every particle :

		if (m_ParticlesArray[i].isActive)
		{
			//1. If the particle is currently Active > UpdateParticle function. (Hint this call needs the
			//elapsedTime, cache this value outside the loop because it’s the same for every
			//particle)

			UpdateParticle(m_ParticlesArray[i], elapsedTime);
		}
		if (!m_ParticlesArray[i].isActive && m_LastParticleSpawn >= particleInterval)
		{
			//2. If the particle is NOT Active(do not use ‘else if’), and the value of m_LastParticleInit is
			//bigger than or equal to particleInterval > Call SpawnParticle.Reason for not using
			//‘else if’ is that a particle can become inactive during the update cycle, meaning we
			//can immediately respawn that particle.

			SpawnParticle(m_ParticlesArray[i]);

		}

		//3. If(after potential update and /or spawn) the particle is Active, add the particle to the
		//VertexBuffer, use m_ActiveParticles as index.After that we increase
		//m_ActiveParticles by one.
		if (m_ParticlesArray[i].isActive)
		{
			pBuffer[m_ActiveParticles] = m_ParticlesArray[i].vertexInfo;
			m_ActiveParticles++;
		}
	}

	//g.Use DeviceContext::Unmap to unmap our vertexbuffer.
	sceneContext.d3dContext.pDeviceContext->Unmap(m_pVertexBuffer, 0);
}

void ParticleEmitterComponent::UpdateParticle(Particle& p, float elapsedTime) const
{
	//TODO_W9(L"Implement UpdateParticle")
	//1. Check if the particle is active, if not, return 
	if (!p.isActive)
	{
		return;
	}

	//2. Subtract the elapsedTime from the particle’s currentEnergy. 
		//a. If currentEnergy is smaller than ZERO, deactivate the particleand return
	p.currentEnergy -= elapsedTime;
	if (p.currentEnergy < 0)
	{
		p.isActive = false;
		return;
	}

	//3. The update method will update all the variables of our VertexParticle parameter 
		//a.vertexInfo.Position
			//i.Add the velocity(m_EmitterSettings) multiplied by the elapsedTime, this way our
			//particle moves in the direction of the velocity defined by the emitter settings.
	
	XMStoreFloat3(&p.vertexInfo.Position, XMLoadFloat3(&p.vertexInfo.Position) + (XMLoadFloat3(&m_EmitterSettings.velocity) * elapsedTime));
	
	

	//Create a local variable, called ‘lifePercent of type float, this is the percentual particle lifetime. This 
	//value can be obtained by dividing the particle’s ‘currentEnergy’ by its ‘totalEnergy’.[At start :
	//lifePercent = 1, At end : lifePercent = 0]
	float lifePercent = p.currentEnergy / p.totalEnergy;

	//b.vertexInfo.Color
	//i.Our color equals the color given by the emitter settings
	p.vertexInfo.Color = m_EmitterSettings.color;

	//ii.The alpha value of the particle color should fade out over time.Use the initial
	//alpha value(m_EmitterSettings.Color.w) multiplied with ‘lifePercent’ additionally
	//you can multiply with an extra constant(like 2) to delay the fade out effect.
	p.vertexInfo.Color.w = lifePercent * 2.0f;

	//c.vertexInfo.Size
	//i.Based on the particle’s sizeChange value, our particle shrinks or grows over time.
	if (p.sizeChange < 1.0)
	{
		//ii.If sizeChange is smaller than 1 (Shrink)
		//1. Example: 0.5 > half its initial size at the end of its life
		//2. Calculate the value for vertexInfo.Size using the Initial size(initialSize),
		//the size grow(sizeChange) and the particle’s life percentage(lifePercent).
		//[Use a sheet of paper to create a visual representation]
		p.vertexInfo.Size = p.initialSize + p.sizeChange * (1 - lifePercent);
	}
	else
	{
		//iii.If sizeChange is bigger than 1 (Grow)
		//1. Example: 2 > double its initial size at the end of its life
		p.vertexInfo.Size = p.initialSize + p.sizeChange * lifePercent;
	}
}

void ParticleEmitterComponent::SpawnParticle(Particle& p)
{
	//TODO_W9(L"Implement SpawnParticle")
	//1. Set particle’s isActive to true
	p.isActive = true;

	//2. Energy Initialization 
	//a.totalEnergy and currentEnergy of the particle are both equal to a random float between
	//minEnergy and maxEnergy(see m_EmitterSettings) [Random float > MathHelper::RandF(...)]
	p.totalEnergy = MathHelper::randF(m_EmitterSettings.minEnergy, m_EmitterSettings.maxEnergy);
	p.currentEnergy = p.totalEnergy;

	//3. Position Initialization 
	//a.We need to calculate a random position; this position is determined by the emitter radius
	//of our particle system.But before we can talk about a radius, we need a random direction.
	XMVECTOR randomDir{ 1,0,0 };

	//b.There are several ways to calculate a random directionand the following is one of the
	//most straightforward way to do it:
		//i.We start by defining a unit vector. (randomDirection = (1, 0, 0))
		//ii.We are going to rotate this unit vector using a random generated rotation matrix,
		//this way we obtain a random normalized vector.Use the
		//XMMatrixRotationRollPitchYaw(...) to create a random rotation matrix(called
		//	randomRotationMatrix). (use RandF(-XM_PI, XM_PI) to generate values for Yaw,
		//		Pitchand Roll).
		//iii.Now we need to transform our randomDirection vector with our
		//randomRotationMatrix. (XMVector3TransformNormal).After this step we created
		//a random normalized vector.
	randomDir = XMVector3TransformNormal(randomDir, XMMatrixRotationRollPitchYaw(MathHelper::randF(-XM_PI, XM_PI), MathHelper::randF(-XM_PI, XM_PI), MathHelper::randF(-XM_PI, XM_PI)));

	//c.We’ve already got our direction; the second step is calculating the distance of our particle
	//starting point.The distance is determined by the minEmitterRangeand the
	//maxEmitterRange(see EmitterSettings).Make sure it is a random value that lays between
	//those two bounds
	float randomDistance{ MathHelper::randF(m_EmitterSettings.minEmitterRadius, m_EmitterSettings.maxEmitterRadius) };

	//d.Everything is in place to calculate the initial position
		//i.vertexInfo.Position = ‘our random direction’ * ‘our random distance’
	XMStoreFloat3(&p.vertexInfo.Position, XMLoadFloat3(&GetTransform()->GetWorldPosition()) + randomDir * randomDistance);

	//4. Size Initialization 
	//a.Our vertexInfo.Size and initialSize are both equal to a random value that lays between MinSize and MaxSize(see EmitterSettings)
	p.vertexInfo.Size = MathHelper::randF(m_EmitterSettings.minSize, m_EmitterSettings.maxSize);
	p.initialSize = p.vertexInfo.Size;

	//b.sizeChange is equal to a random value that lays between minScaleand maxScale(see EmitterSettings)
	p.sizeChange = MathHelper::randF(m_EmitterSettings.minScale, m_EmitterSettings.maxScale);

	//5. Rotation Initialization 
	//a.The rotation(vertexInfo.Rotation) is a random value between –PIand PI.
	p.vertexInfo.Rotation = MathHelper::randF(-XM_PI, XM_PI);

	//6. Color Initialisation
	//a.The particle’s color(vertexInfo.Color) is equal to the color from the emitter settings.
	p.vertexInfo.Color = m_EmitterSettings.color;
}

void ParticleEmitterComponent::PostDraw(const SceneContext& sceneContext)
{
	//TODO_W9(L"Implement PostDraw")
	//1. Set the following shader variables to our ParticleMaterial(m_pParticleMaterial)
	//a.gWorldViewProj > Camera ViewProjection(we don’t need to pass the actual
	//	WorldViewProjection.Our particles already live in World - Space, so we don’t need to
	//	transform them again inside the shader.That’s why we only need to pass the camera’s
	//	ViewProjection as WVP variable)
	m_pParticleMaterial->SetVariable_Matrix(L"gWorldViewProj", sceneContext.pCamera->GetViewProjection());

	//b.gViewInverse > Camera ViewInverse
	m_pParticleMaterial->SetVariable_Matrix(L"gViewInverse", sceneContext.pCamera->GetViewInverse());

	//c.gParticleTexture > m_pParticleTexture
	m_pParticleMaterial->SetVariable_Texture(L"gParticleTexture", m_pParticleTexture);

	//2. Retrieve the TechniqueContext from the material, this structure contains relevant information to
	//setup the pipeline(BaseMaterial::GetTechniqueContext)

	MaterialTechniqueContext tech = m_pParticleMaterial->GetTechniqueContext();

	//3. Set the InputLayout
	sceneContext.d3dContext.pDeviceContext->IASetInputLayout(tech.pInputLayout);

	//4. Set the PrimitiveTopology
	//	a.Remember we are only using points for our particles, so select the appropriate topology.
	sceneContext.d3dContext.pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

	//5. Set the VertexBuffer
	//	a.We have no startslot and only one buffer
	//	b.We have no offset
	//	c.The size of one vertex(stride) is equal to the size of VertexParticle
	UINT stride{ sizeof(VertexParticle) };
	UINT offset{ 0 };
	sceneContext.d3dContext.pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	D3DX11_TECHNIQUE_DESC techDesc;
	tech.pTechnique->GetDesc(&techDesc);
	for (UINT i{ 0 }, passes{ techDesc.Passes }; i < passes; ++i)
	{
		//6. For each pass of our technique
		//	a.Apply the pass
		//	b.Draw the vertices!(The number of vertices we want to draw is equal to m_ActiveParticles)

		tech.pTechnique->GetPassByIndex(i)->Apply(0, sceneContext.d3dContext.pDeviceContext);
		sceneContext.d3dContext.pDeviceContext->Draw(m_ActiveParticles, 0);
	}
}

void ParticleEmitterComponent::DrawImGui()
{
	if(ImGui::CollapsingHeader("Particle System"))
	{
		ImGui::SliderUInt("Count", &m_ParticleCount, 0, m_MaxParticles);
		ImGui::InputFloatRange("Energy Bounds", &m_EmitterSettings.minEnergy, &m_EmitterSettings.maxEnergy);
		ImGui::InputFloatRange("Size Bounds", &m_EmitterSettings.minSize, &m_EmitterSettings.maxSize);
		ImGui::InputFloatRange("Scale Bounds", &m_EmitterSettings.minScale, &m_EmitterSettings.maxScale);
		ImGui::InputFloatRange("Radius Bounds", &m_EmitterSettings.minEmitterRadius, &m_EmitterSettings.maxEmitterRadius);
		ImGui::InputFloat3("Velocity", &m_EmitterSettings.velocity.x);
		ImGui::ColorEdit4("Color", &m_EmitterSettings.color.x, ImGuiColorEditFlags_NoInputs);
	}
}