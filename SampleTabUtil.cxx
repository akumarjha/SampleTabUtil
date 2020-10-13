#include "SampleTabUtil.hxx"

#include <IntegerVar.hxx>
#include <LangTextVar.hxx>
#include <AnyTypeVar.hxx>
#include <TextVar.hxx>
#include <CharVar.hxx>
#include <BitVar.hxx>
#include <StrUtil.hxx>
#include <AliasTable.hxx>
#include <DpIdentifier.hxx>
#include <Datapoint.hxx>
#include <DpIdentifierItem.hxx>
#include <DpIdentification.hxx>
#include <Resources.hxx>
#include <DpIdentifierVar.hxx>
#include <DpElement.hxx>
#include <DpTypeContainer.hxx>
#include <DpType.hxx>
#include <iostream> 
//using namespace AliasTable;
//using namespace AliasTableItem;
using namespace std;

//--------------------------------------------------------------------------------

TABUTIL_EXPORT int tabUtilTranslateDirection
(
	const DynVar& in, // Arguments passed to the function
	DynVar& out       // Return value of the function
)
{
	// Local data
	IntegerVar* pAckStateValue; // First argument is the _ack_state
	BitVar* pDirectionBit; // Second argument is the _direction
	int t; //Counter to go through the arguments passed to this function
	char szBuffer[100]; //Buffer used to format the response
	CharString strActive; /* Text that is shown when an item is active/inactive */
	CharString strAcked; // Text that is shown when an item is acknowledged
	bool bFound;

	//2 valid arguments are expected
	//Assure that valid pointers with valid data type are returned
	for (t = 1; t <= 2; t++)
	{
		if (!in[t]) //if there is no valid pointer
			return -1; // then exit

		if (in[t]->isA() != ANYTYPE_VAR) /* All arguments should be ANYTYPE_VAR */
			return -1; //else just exit
	}

	//Convert the first argument to an INTEGER_VAR.
	//This will be the _ack_state.
	if (((AnyTypeVar*)in[1])->getVar()->convert(INTEGER_VAR, (Variable*&)pAckStateValue) != Variable::OK)
	{
		return -1;
	}

	/* Convert the second argument to a BIT_VAR. This will be the direction "Came" or "Went"*/
	if (((AnyTypeVar*)in[2])->getVar()->convert(BIT_VAR, (Variable*&)pDirectionBit) != Variable::OK)
	{
		/* when the conversion fails then make sure to delete the arguments allocated earlier */
		delete pAckStateValue;
		return -1;
	}
	/* Locate the appropriate language entry in the catalogue file "MYAES.CAT"*/
	//the right text is needed for "Active/Inactive" and "Acked/Unacked"
	strActive = StrUtil::catgets("MYAES", (PVSSint)*pDirectionBit ? "active" : "inactive", bFound);
	if (!bFound)
		strActive = "Catalogue file ?";

	strAcked = StrUtil::catgets("MYAES", (PVSSint)*pAckStateValue ? "acked" : "not_acked", bFound);
	if (!bFound)
		strAcked = "Catalogue file ?";

	//Format the actual string that should be shown in the AES panel
	sprintf(szBuffer, "%s - %s", (const char*)strActive, (const char*)strAcked);

	//Allocate the string that should be returned
	out.append(new AnyTypeVar(new TextVar(szBuffer)));

	//Free any allocated variables
	delete pDirectionBit;
	delete pAckStateValue;

	return 1;
}

//--------------------------------------------------------------------------------

TABUTIL_EXPORT int tabUtilConcate(const DynVar& in, DynVar& out)
{
	Variable* pStringVar1 = in[1];
	Variable* pStringVar2 = in[2];

	if (pStringVar1 && pStringVar1->isA() == ANYTYPE_VAR &&
		pStringVar2 && pStringVar2->isA() == ANYTYPE_VAR)
	{
		Variable* pString1 = ((AnyTypeVar*)pStringVar1)->getVar();
		Variable* pString2 = ((AnyTypeVar*)pStringVar2)->getVar();

		CharString s1, s2;

		if (pString1 && pString2)
		{
			if (pString1->isA() == TEXT_VAR)
				s1 = ((TextVar*)pString1)->getValue();
			else if (pString1->isA() == LANGTEXT_VAR)
				s1 = ((LangTextVar*)pString1)->getValue().getText();

			if (pString2->isA() == TEXT_VAR)
				s2 = ((TextVar*)pString2)->getValue();
			else if (pString2->isA() == LANGTEXT_VAR)
				s2 = ((LangTextVar*)pString2)->getValue().getText();

			out.append(new AnyTypeVar(new TextVar(s1 + s2)));

			return 1; // Success
		}
	}
	return -1;  //Failure
}

//--------------------------------------------------------------------------------

TABUTIL_EXPORT int tabUtilBar(const DynVar& in, DynVar& out)
{
	// parameter1  CharVar
	// parameter 2 LangTextVar

	Variable* pIntVar = in[1];
	Variable* pTokenVar = in[2];

	if (pIntVar && pIntVar->isA() == ANYTYPE_VAR &&
		pTokenVar && pTokenVar->isA() == ANYTYPE_VAR)
	{
		Variable* pInt = ((AnyTypeVar*)pIntVar)->getVar();
		Variable* pToken = ((AnyTypeVar*)pTokenVar)->getVar();

		if (pInt && pInt->isA() == CHAR_VAR &&
			pToken && pToken->isA() == LANGTEXT_VAR)
		{
			if (pIntVar && pTokenVar)
			{
				CharString output;
				CharString token = ((LangTextVar*)pToken)->getValue().getText();
				int num = (int)((CharVar*)pInt)->getValue();

				for (int i = 0; i < num / 10; i++)
					output += token;

				out.append(new AnyTypeVar(new TextVar(output)));

				return 1; // Success
			}
		}
	}
	return -1; //Failure
}

int findLastIndex(string& str, char x)
{
	int index = -1;
	for (int i = 0; i < str.length(); i++)
		if (str[i] == x)
			index = i;
	return index;
}

TABUTIL_EXPORT int tabUtilLastComment(const DynVar& in, DynVar& out)
{
	Variable* pStringVar1 = in[1];
	//Variable* pStringVar2 = in[2];

	if (pStringVar1 && pStringVar1->isA() == ANYTYPE_VAR)
	{
		Variable* pString1 = ((AnyTypeVar*)pStringVar1)->getVar();
		//Variable* pString2 = ((AnyTypeVar*)pStringVar2)->getVar();

		CharString s1;

		if (pString1)
		{
			if (pString1->isA() == TEXT_VAR)
				s1 = ((TextVar*)pString1)->getValue();
			else if (pString1->isA() == LANGTEXT_VAR)
				s1 = ((LangTextVar*)pString1)->getValue().getText();

			/*  if (pString2->isA() == TEXT_VAR)
				  s2 = ((TextVar*)pString2)->getValue();
			  else if (pString2->isA() == LANGTEXT_VAR)
				  s2 = ((LangTextVar*)pString2)->getValue().getText();*/

				  //CharString token, strtoken;
				  //token = strtok(s1, "|");            
				  //while (token != (CharString)NULL)
				  //{
				  //    //printf("while token %s", token);
				  //    //writeToFile(token);
				  //    strtoken = token;
				  //    token = strtok(NULL, "|");
				  //}
			string st = s1;

			//size_t len = size(s1);
			//size_t lastidx = s1.indexOfOneOf("|", 0);
			int idx = findLastIndex(st, '|');
			int strlen = st.length();

			CharString strComment = s1.substring(idx + 1, (strlen - idx - 3)); //st.substr(idx, strlen);
			//CharString cs = 
			//CharString cs = (car)strComment;
			out.append(new AnyTypeVar(new TextVar(strComment)));

			return 1; // Success
		}
	}
	return -1;  //Failure
}

//TABUTIL_EXPORT int tabUtilCustomerNumber(const DynVar& in, DynVar& out)
//{
//	Variable* pStringVar1 = in[1];
//
//	if (pStringVar1 && pStringVar1->isA() == ANYTYPE_VAR)
//	{
//		Variable* pString1 = ((AnyTypeVar*)pStringVar1)->getVar();
//
//		CharString s1;
//
//		if (pString1)
//		{
//			if (pString1->isA() == TEXT_VAR)
//				s1 = ((TextVar*)pString1)->getValue();
//			else if (pString1->isA() == LANGTEXT_VAR)
//				s1 = ((LangTextVar*)pString1)->getValue().getText();
//
//			//string st = s1;
//
//			DpIdentifier di;
//			char* charstr = s1;
//			char* token = strtok(s1, ".");
//			
//			DpIdentifier dpi = di.convertFromString(token);
//			dpi.toString() + ".";
//			LangText alias;
//			AliasTable ati;
//			ati.getAlias(dpi, alias);
//
//			/*CharString*/
//
//			CharString strGroup = alias.getText();
//			//size_t len = size(s1);
//			//size_t lastidx = s1.indexOfOneOf("|", 0);
//
//
//
//		/*	int idx = findLastIndex(st, '|');
//			int strlen = alis*/
//
//
//				//CharString strComment = s1.substring(idx + 1, (strlen - idx - 3)); //st.substr(idx, strlen);
//				//CharString cs = 
//				//CharString cs = (car)strComment;
//			out.append(new AnyTypeVar(new TextVar(s1)));
//
//			return 1; // Success
//		}
//	}
//	return -1;  //Failure
//}


//DynPtrArrayIndex getName(const DpIdentifier& id, CharString& name)
//{
//	MyIdentificationItem tmp;
//	tmp.setDpId(id);
//
//	DynPtrArrayIndex res = byId.findItem(&tmp);
//	if (res != DYNPTRARRAY_INVALID)
//	{
//		name = byId.getAt(res)->getName();
//	}
//
//	return res;
//}

TABUTIL_EXPORT int tabUtilCustomerNumber(const DpIdentifier& in, DynVar& out)
{
	////set number of languages
	//Resources::setNumOfLangs(1);

	////created type container
	//SystemNumType system = in.getSystem();
	//DpTypeContainer* ptr = DpTypeContainer::getTypeContainerPtr(system);  
	//DpIdentification dpi;
	//dpi.setTypeContainer(ptr);

	////setting default system
	//dpi.setDefaultSystem(system);

	////set Type Name
	//DpTypeId dptId = in.getDpType();
	//char* typeName;
	//dpi.getTypeName(dptId, typeName);
	//dpi.addTypeName(dptId, typeName);

	////add Datapoint Name
	//DpIdType id = in.getDp();
	//char* dpName;
	//dpi.getDpName(id, dpName);
	//dpi.addDatapointName(id, dptId, dpName);

	////add Element Name
	//DpElementId dpeId = in.getEl();
	//char* dpeName;
	//dpi.getElementName(dptId, dpeId, dpeName);
	//dpi.addElementName(dptId, dpeId, dpeName);

	//CharString alias;
	//dpi.getDpAlias(dpeName, alias);

	CharString alias;
	DpIdentification dpi = new DpIdentification();
	dpi.setDefaultSystem(dpi.getDefaultSystem());
	DpIdentificationResult dpr = dpi.getDpAlias(in, alias);
	
	out.append(new AnyTypeVar(new TextVar(alias +"---"+dpr)));
	return 1;
}