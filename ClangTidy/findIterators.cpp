#include "findIterators.hpp"

// http://blog.audio-tk.com/2018/03/20/writing-custom-checks-for-clang-tidy/
// https://clang.llvm.org/docs/LibASTMatchersReference.html
// https://devblogs.microsoft.com/cppblog/exploring-clang-tooling-part-2-examining-the-clang-ast-with-clang-query/

namespace clang {
    namespace tidy {
	auto boundExpr(std::string const& bindName) {
	    return expr().bind(bindName);
	}

	auto boundLocalVar(std::string const& bindName, auto... constraints) {
	    return varDecl(decl().bind(bindVarName), hasAutomaticStorageDuration(), constraints...);
	}
	
	auto varUse(auto... varConstraints) {
	    return declRefExpr(to(varConstraints...));
	}
	    
	auto memberCallMatcher(auto thisExprMatcher, std::string const& methodName) {
	    auto theMethod = callee(hasName(methodName));
	    auto theMethodCall = cxxMemberCallExpr(theMethod, on(thisExprMatcher));
	    return theMethodCall;
	}
	
	void FindIterators::registerMatchers(ast_matchers::MatchFinder* finder) {
	    if (!getLangOpts().CPlusPlus)
		return;

	    // CG::StmtIterator itr(region); itr.hasCurrent(); itr.advance()
	    // auto it = data->iterations(); it.hasCurrent(); it.advance()
	    // EmlEntryFunctionIterator i(fEnv); i.hasCurrent(); i.advance()
	    // RecursiveChildFactsIterator iter(fact); iter.hasCurrent(); iter.advance()
	    // CG::IOVarIterator ov(fFcn); ov.hasCurrent(); ov.advance()
	    // CG::CfgNodeListIterator<CG::VarExpr> iter(fcn()->cfg()); iter.hasCurrent(); iter.advance()
	    // CG::ScopeTreeIterator<CG::Var> iter(aFcn->localScope()); iter.hasCurrent(); iter.advance()
	    // DefMapType::IteratorType iter = defs.iterator(); iter.hasCurrent(); iter.advance()
	    // auto startUses = fUseDef->uses(fTranslator.cfg()->start());
	    // for (; startUses.hasCurrent(); startUses.advance())
	    // EmlUseDef::NodeIterator iter(fUseDef->defs(fCfg->end()));
	    // for (; iter.hasCurrent(); iter.advance())

	    
	    // iteratorVar may only be used in the loop, and only in ".current()" calls
	    auto range = boundExpr("range");
	    auto iteratorCall = memberCallMatcher(range, "iterator");
	    auto iterationsCall = memberCallMatcher(range, "iterations");
	    auto beginCall = memberCallMatcher(range, "begin");
	    auto rangeIterator = anyOf(iteratorCall, iterationsCall, beginCall, range);
	    
	    auto iteratorVar = boundLocalVar("theIterator", hasInitializer(rangeIterator));
	    auto iteratorVarUse = varUse(iteratorVar);
	    
	    auto hasCurrentCall = memberCallMatcher(iteratorVarUse, "hasCurrent");
	    auto currentCall = memberCallMatcher(iteratorVarUse, "current");
	    auto advanceCall = memberCallMatcher(iteratorVarUse, "advance");

	    auto forLoop = forStmt();
	    
	    auto iteratorVarUseInLoop = allOf(iteratorVarUse, hasAncestor(forLoop))

	    finder->addMatcher(forLoop);
	}

	void FindIterators::check(MatchFinder::MatchResult const& result) {
	    auto range = result.Nodes.getNode("range");
	    auto loop = result.Nodes.getNode("loop");
	}
    }
}
