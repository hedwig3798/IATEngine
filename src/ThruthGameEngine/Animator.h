#pragma once
#include "Component.h"
#include "imgui.h"
#include "imgui_internal.h"
#include "GraphEditor.h"

namespace Truth
{
	class SkinnedMesh;
}


namespace Truth
{
	class Animator :
		public Component
	{
		GENERATE_CLASS_TYPE_INFO(Animator);
	private:
		friend class boost::serialization::access;
		BOOST_SERIALIZATION_SPLIT_MEMBER();
		template<class Archive>
		void save(Archive& ar, const unsigned int file_version) const;
		template<class Archive>
		void load(Archive& ar, const unsigned int file_version);

	public:
		std::weak_ptr<Truth::SkinnedMesh> m_skinnedMesh;

	public:
		template <typename T, std::size_t N>
		struct Array
		{
			T data[N];
			const size_t size() const { return N; }

			const T operator [] (size_t index) const { return data[index]; }
			operator T* () {
				T* p = new T[N];
				memcpy(p, data, sizeof(data));
				return p;
			}
		};

		struct AnimatorNode
		{
			std::string name = "Empty";
			GraphEditor::TemplateIndex templateIndex = 0;
			float x = 0, y = 0;
			bool mSelected = false;

			bool isAnimationPlayed = false;
			std::wstring animationPath = L"";
			bool isChangesOnFinished = false;

			float animationSpeed = 1.f;
		};

		template <typename T, typename ... U> Array(T, U...) -> Array<T, 1 + sizeof...(U)>;

		struct GraphEditorDelegate : public GraphEditor::Delegate
		{
			Animator* m_animator = nullptr;

			bool AllowedLink(GraphEditor::NodeIndex from, GraphEditor::NodeIndex to) override
			{
				return true;
			}

			void SelectNode(GraphEditor::NodeIndex nodeIndex, bool selected) override
			{
				m_animationNodes[nodeIndex].mSelected = selected;
			}

			void MoveSelectedNodes(const ImVec2 delta) override
			{
				for (auto& node : m_animationNodes)
				{
					if (!node.mSelected)
					{
						continue;
					}
					node.x += delta.x;
					node.y += delta.y;
				}
			}

			virtual void RightClick(GraphEditor::NodeIndex nodeIndex, GraphEditor::SlotIndex slotIndexInput, GraphEditor::SlotIndex slotIndexOutput) override
			{

			}

			void AddLink(GraphEditor::NodeIndex inputNodeIndex, GraphEditor::SlotIndex inputSlotIndex, GraphEditor::NodeIndex outputNodeIndex, GraphEditor::SlotIndex outputSlotIndex) override
			{
				if (inputNodeIndex != outputNodeIndex)
				{
					mLinks.push_back({ inputNodeIndex, inputSlotIndex, outputNodeIndex, outputSlotIndex });
				}
			}

			void DelLink(GraphEditor::LinkIndex linkIndex) override
			{
				mLinks.erase(mLinks.begin() + linkIndex);
			}

			void CustomDraw(ImDrawList* drawList, ImRect rectangle, GraphEditor::NodeIndex nodeIndex) override;


			const size_t GetTemplateCount() override
			{
				return sizeof(mTemplates) / sizeof(GraphEditor::Template);
			}

			const GraphEditor::Template GetTemplate(GraphEditor::TemplateIndex index) override
			{
				return mTemplates[index];
			}

			const size_t GetNodeCount() override
			{
				return m_animationNodes.size();
			}

			const GraphEditor::Node GetNode(GraphEditor::NodeIndex index) override
			{
				const auto& myNode = m_animationNodes[index];
				return GraphEditor::Node
				{
					myNode.name.c_str(),
					myNode.templateIndex,
					ImRect(ImVec2(myNode.x, myNode.y), ImVec2(myNode.x + 200, myNode.y + 70)),
					myNode.mSelected
				};
			}

			const size_t GetLinkCount() override
			{
				return mLinks.size();
			}

			const GraphEditor::Link GetLink(GraphEditor::LinkIndex index) override
			{
				return mLinks[index];
			}

			// Graph datas
			static const inline GraphEditor::Template mTemplates[] = {
				{
					IM_COL32(160, 160, 180, 255),
					IM_COL32(100, 100, 140, 255),
					IM_COL32(110, 110, 150, 255),
					1,
					{Array<const char*, 1>{"Input"}},
					nullptr,
					1,
					{Array<const char*, 1>{"Output"}},
					nullptr
				},

			};



			std::vector<AnimatorNode> m_animationNodes = {
				{
					"Entry",
					0,
					50, 100,
					false
				},
				{
					"Any State",
					0,
					50, 400,
					false
				},
				{
					"Exit",
					0,
					50, 700,
					false
				}
			};

			std::vector<GraphEditor::Link> mLinks/* = { {0, 0, 0, 0} }*/;
		};

	public:
		GraphEditor::Options options;
		GraphEditorDelegate delegate;
		GraphEditor::ViewState viewState;
		GraphEditor::FitOnScreen fit;
		bool showGraphEditor;

	public:
		Animator();
		virtual ~Animator();

		METHOD(Initialize);
		void Initialize();

		METHOD(Awake);
		void Awake();

		METHOD(Start);
		void Start();

		METHOD(Update);
		void Update();

		METHOD(Destroy);
		virtual void Destroy() override;

	private:
		AnimatorNode m_currentNode;

	};

	template<class Archive>
	void Truth::Animator::load(Archive& _ar, const unsigned int file_version)
	{
		_ar& boost::serialization::base_object<Component>(*this);

	}

	template<class Archive>
	void Truth::Animator::save(Archive& _ar, const unsigned int file_version) const
	{
		_ar& boost::serialization::base_object<Component>(*this);

	}
}



BOOST_CLASS_EXPORT_KEY(Truth::Animator)
BOOST_CLASS_VERSION(Truth::Animator, 0)