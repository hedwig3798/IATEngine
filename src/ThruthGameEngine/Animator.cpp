#include "Animator.h"
#include "SkinnedMesh.h"

BOOST_CLASS_EXPORT_IMPLEMENT(Truth::Animator)

// GraphEditor::Options Truth::Animator::options;
// Truth::GraphEditorDelegate Truth::Animator::delegate;
// GraphEditor::ViewState Truth::Animator::viewState;
// GraphEditor::FitOnScreen Truth::Animator::fit = GraphEditor::Fit_None;
// bool Truth::Animator::showGraphEditor = true;

Truth::Animator::Animator()
	: showGraphEditor(true)
	, fit(GraphEditor::Fit_None)
{
	delegate.m_animator = this;
}

Truth::Animator::~Animator()
{
}

void Truth::Animator::Initialize()
{
	m_skinnedMesh = m_owner.lock().get()->GetComponent<Truth::SkinnedMesh>();
}

void Truth::Animator::Awake()
{

}

void Truth::Animator::Destroy()
{
	m_skinnedMesh.reset();
}

void Truth::Animator::Start()
{
	for (auto& e : delegate.m_animationNodes)
	{
		if (e.animationPath != L"")
		{
			m_skinnedMesh.lock()->AddAnimation(e.name, e.animationPath);
		}
	}
	m_currentNode = delegate.m_animationNodes[0];
}

void Truth::Animator::Update()
{
	if (m_currentNode.name == delegate.m_animationNodes[0].name)
	{
		m_currentNode = delegate.m_animationNodes[delegate.mLinks[0].mOutputNodeIndex];
	}

// 	if (m_skinnedMesh->m_isAnimationEnd)
// 	{
// 		m_skinnedMesh->SetAnimation(m_currentNode.name, m_currentNode.isChangesOnFinished);
// 		//m_currentNode.isAnimationPlayed = false;
// 	}

}

void Truth::Animator::GraphEditorDelegate::CustomDraw(ImDrawList* drawList, ImRect rectangle, GraphEditor::NodeIndex nodeIndex)
{
	/// ProgressBar
	int a = 0;
	std::vector<GraphEditor::Link> xLinks = mLinks;
	int b = 0;
	if (m_animationNodes[nodeIndex].isAnimationPlayed)
	{
// 		float progress = (float)m_animator->m_skinnedMesh->m_currentFrame;
// 		float progress_dir = (float)m_animator->m_skinnedMesh->m_animationMaxFrame;
// 
// 
// 		ImVec2 lenVec(rectangle.Max.x - rectangle.Min.x, rectangle.Max.y - rectangle.Min.y);
// 		ImVec2 pVec;
// 		pVec.x = rectangle.Min.x + lenVec.x * (progress / progress_dir);
// 		pVec.y = rectangle.Max.y - 30.f;
// 		drawList->AddRectFilled(rectangle.Min, pVec, IM_COL32(155, 155, 255, 255));

	}
}
