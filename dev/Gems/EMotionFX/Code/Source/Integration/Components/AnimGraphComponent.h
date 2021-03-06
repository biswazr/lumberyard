/*
* All or portions of this file Copyright (c) Amazon.com, Inc. or its affiliates or
* its licensors.
*
* For complete copyright and license terms please see the LICENSE at the root of this
* distribution (the "License"). All use of this software is governed by the License,
* or, if provided, by the license below or the license accompanying this file. Do not
* remove or modify any license notices. This file is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
*
*/


#pragma once

#include <AzCore/Component/Component.h>
#include <AzCore/Component/TransformBus.h>
#include <AzCore/Component/TickBus.h>
#include <AzCore/Asset/AssetCommon.h>
#include <AzCore/Script/ScriptProperty.h>
#include <AzCore/std/smart_ptr/unique_ptr.h>

#include <Integration/Assets/AnimGraphAsset.h>
#include <Integration/Assets/MotionSetAsset.h>
#include <Integration/ActorComponentBus.h>
#include <Integration/AnimGraphComponentBus.h>

namespace EMotionFX
{
    namespace Integration
    {
        class AnimGraphComponent
            : public AZ::Component
            , private AZ::Data::AssetBus::MultiHandler
            , private ActorComponentNotificationBus::Handler
            , private AnimGraphComponentRequestBus::Handler
        {
        public:

            friend class EditorAnimGraphComponent;

            AZ_COMPONENT(AnimGraphComponent, "{77624349-D5C4-4902-9F08-665814520999}");

            /**
             * Structure containing data-driven properties extracted from the anim graph,
             * to allow override control per-entity via the component inspector UI.
             */
            struct ParameterDefaults
            {
                AZ_TYPE_INFO(ParameterDefaults, "{E6826EB9-C79B-43F3-A03F-3298DD3C724E}")

                    ParameterDefaults();
                ~ParameterDefaults();
                ParameterDefaults& operator=(const ParameterDefaults& rhs)
                {
                    Reset();

                    m_parameters.reserve(rhs.m_parameters.size());

                    for (AZ::ScriptProperty* p : rhs.m_parameters)
                    {
                        m_parameters.push_back(p->Clone());
                    }

                    return *this;
                }

                typedef AZStd::vector<AZ::ScriptProperty*> ParameterList;
                ParameterList   m_parameters;

                void Reset();
                static void Reflect(AZ::ReflectContext* context);
            };

            /**
            * Configuration struct for procedural configuration of Actor Components.
            */
            struct Configuration
            {
                AZ_TYPE_INFO(Configuration, "{F5A93340-60CD-4A16-BEF3-1014D762B217}")

                    Configuration();

                AZ::Data::Asset<AnimGraphAsset>    m_animGraphAsset;          ///< Selected anim graph.
                AZ::Data::Asset<MotionSetAsset>     m_motionSetAsset;           ///< Selected motion set.
                ParameterDefaults                   m_parameterDefaults;        ///< Defaults for parameter values.

                static void Reflect(AZ::ReflectContext* context);
            };

            AnimGraphComponent(const Configuration* config = nullptr);
            ~AnimGraphComponent() override;

            //////////////////////////////////////////////////////////////////////////
            // AZ::Component interface implementation
            void Init() override;
            void Activate() override;
            void Deactivate() override;
            //////////////////////////////////////////////////////////////////////////

            //////////////////////////////////////////////////////////////////////////
            // AnimGraphComponentRequestBus::Handler
            EMotionFX::AnimGraphInstance* GetAnimGraphInstance() override { return m_animGraphInstance.get(); }
            AZ::u32 FindParameterIndex(const char* parameterName) override;
            void SetParameterFloat(AZ::u32 parameterIndex, float value) override;
            void SetParameterBool(AZ::u32 parameterIndex, bool value) override;
            void SetParameterString(AZ::u32 parameterIndex, const char* value) override;
            void SetParameterVector2(AZ::u32 parameterIndex, const AZ::Vector2& value) override;
            void SetParameterVector3(AZ::u32 parameterIndex, const AZ::Vector3& value) override;
            void SetParameterRotationEuler(AZ::u32 parameterIndex, const AZ::Vector3& value) override;
            void SetParameterRotation(AZ::u32 parameterIndex, const AZ::Quaternion& value) override;
            void SetNamedParameterFloat(const char* parameterName, float value) override;
            void SetNamedParameterBool(const char* parameterName, bool value) override;
            void SetNamedParameterString(const char* parameterName, const char* value) override;
            void SetNamedParameterVector2(const char* parameterName, const AZ::Vector2& value) override;
            void SetNamedParameterVector3(const char* parameterName, const AZ::Vector3& value) override;
            void SetNamedParameterRotationEuler(const char* parameterName, const AZ::Vector3& value) override;
            void SetNamedParameterRotation(const char* parameterName, const AZ::Quaternion& value) override;
            float GetParameterFloat(AZ::u32 parameterIndex) override;
            bool GetParameterBool(AZ::u32 parameterIndex) override;
            AZStd::string GetParameterString(AZ::u32 parameterIndex) override;
            AZ::Vector2 GetParameterVector2(AZ::u32 parameterIndex) override;
            AZ::Vector3 GetParameterVector3(AZ::u32 parameterIndex) override;
            AZ::Vector3 GetParameterRotationEuler(AZ::u32 parameterIndex) override;
            AZ::Quaternion GetParameterRotation(AZ::u32 parameterIndex) override;
            float GetNamedParameterFloat(const char* parameterName) override;
            bool GetNamedParameterBool(const char* parameterName) override;
            AZStd::string GetNamedParameterString(const char* parameterName) override;
            AZ::Vector2 GetNamedParameterVector2(const char* parameterName) override;
            AZ::Vector3 GetNamedParameterVector3(const char* parameterName) override;
            AZ::Vector3 GetNamedParameterRotationEuler(const char* parameterName) override;
            AZ::Quaternion GetNamedParameterRotation(const char* parameterName) override;
            //////////////////////////////////////////////////////////////////////////

            //////////////////////////////////////////////////////////////////////////
            // ActorComponentNotificationBus::Handler
            void OnActorInstanceCreated(EMotionFX::ActorInstance* /*actorInstance*/) override;
            void OnActorInstanceDestroyed(EMotionFX::ActorInstance* /*actorInstance*/) override;
            //////////////////////////////////////////////////////////////////////////

            //////////////////////////////////////////////////////////////////////////
            static void GetProvidedServices(AZ::ComponentDescriptor::DependencyArrayType& provided)
            {
                provided.push_back(AZ_CRC("EMotionFXAnimGraphService", 0x9ec3c819));
            }

            static void GetIncompatibleServices(AZ::ComponentDescriptor::DependencyArrayType& incompatible)
            {
                incompatible.push_back(AZ_CRC("EMotionFXAnimGraphService", 0x9ec3c819));
            }

            static void GetDependentServices(AZ::ComponentDescriptor::DependencyArrayType& /*dependent*/)
            {
            }

            static void GetRequiredServices(AZ::ComponentDescriptor::DependencyArrayType& required)
            {
                required.push_back(AZ_CRC("EMotionFXActorService", 0xd6e8f48d));
            }

            static void Reflect(AZ::ReflectContext* context);
            //////////////////////////////////////////////////////////////////////////

        private:

            // AZ::Data::AssetBus::Handler
            void OnAssetReady(AZ::Data::Asset<AZ::Data::AssetData> asset) override;
            void OnAssetReloaded(AZ::Data::Asset<AZ::Data::AssetData> asset) override;

            void CheckCreateAnimGraphInstance();
            void DestroyAnimGraphInstance();

            // Helper functions to wrap special logic required for EMFX anim graph ref-counting.
            void AnimGraphInstancePostCreate();
            void AnimGraphInstancePreDestroy();

            Configuration                               m_configuration;        ///< Component configuration.

            EMotionFXPtr<EMotionFX::ActorInstance>      m_actorInstance;        ///< Associated actor instance (retrieved from Actor Component).
            EMotionFXPtr<EMotionFX::AnimGraphInstance> m_animGraphInstance;   ///< Live anim graph instance.
        };

    } // namespace Integration
} // namespace EMotionFX

