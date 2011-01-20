/*=========================================================================

  Program:   Visualization Toolkit
  Module:    vtkWrap.c

  Copyright (c) Ken Martin, Will Schroeder, Bill Lorensen
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkWrap.h"
#include <string.h>
#include <ctype.h>

/* -------------------------------------------------------------------- */
/* Common types. */

int vtkWrap_IsVoid(ValueInfo *val)
{
  if (val == 0)
    {
    return 1;
    }

  return ((val->Type & VTK_PARSE_UNQUALIFIED_TYPE) == VTK_PARSE_VOID);
}

int vtkWrap_IsVoidFunction(ValueInfo *val)
{
  unsigned int t = (val->Type & VTK_PARSE_UNQUALIFIED_TYPE);
  return (t == VTK_PARSE_FUNCTION);
}

int vtkWrap_IsVoidPointer(ValueInfo *val)
{
  unsigned int t = (val->Type & VTK_PARSE_BASE_TYPE);
  return (t == VTK_PARSE_VOID && vtkWrap_IsPointer(val));
}

int vtkWrap_IsCharPointer(ValueInfo *val)
{
  unsigned int t = (val->Type & VTK_PARSE_BASE_TYPE);
  return (t == VTK_PARSE_CHAR && vtkWrap_IsPointer(val));
}

int vtkWrap_IsVTKObject(ValueInfo *val)
{
  unsigned int t = (val->Type & VTK_PARSE_UNQUALIFIED_TYPE);
  return (t == VTK_PARSE_OBJECT_PTR &&
          val->Class[0] == 'v' && strncmp(val->Class, "vtk", 3) == 0);
}

int vtkWrap_IsSpecialObject(ValueInfo *val)
{
  unsigned int t = (val->Type & VTK_PARSE_UNQUALIFIED_TYPE);
  return ((t == VTK_PARSE_OBJECT ||
           t == VTK_PARSE_OBJECT_REF) &&
          val->Class[0] == 'v' && strncmp(val->Class, "vtk", 3) == 0);
}

int vtkWrap_IsQtObject(ValueInfo *val)
{
  unsigned int t = (val->Type & VTK_PARSE_BASE_TYPE);
  if (t == VTK_PARSE_QOBJECT &&
      val->Class[0] == 'Q' && isupper(val->Class[1]))
    {
    return 1;
    }
  return 0;
}

int vtkWrap_IsQtEnum(ValueInfo *val)
{
  unsigned int t = (val->Type & VTK_PARSE_UNQUALIFIED_TYPE);
  if ((t == VTK_PARSE_QOBJECT || t == VTK_PARSE_QOBJECT_REF) &&
      val->Class[0] == 'Q' && strncmp("Qt::", val->Class, 4) == 0)
    {
    return 1;
    }
  return 0;
}


/* -------------------------------------------------------------------- */
/* The base types, all are mutually exclusive. */

int vtkWrap_IsObject(ValueInfo *val)
{
  unsigned int t = (val->Type & VTK_PARSE_BASE_TYPE);
  return (t == VTK_PARSE_OBJECT ||
          t == VTK_PARSE_QOBJECT);
}

int vtkWrap_IsFunction(ValueInfo *val)
{
  unsigned int t = (val->Type & VTK_PARSE_BASE_TYPE);
  return (t == VTK_PARSE_FUNCTION);
}

int vtkWrap_IsStream(ValueInfo *val)
{
  unsigned int t = (val->Type & VTK_PARSE_BASE_TYPE);
  return (t == VTK_PARSE_ISTREAM ||
          t == VTK_PARSE_OSTREAM);
}

int vtkWrap_IsNumeric(ValueInfo *val)
{
  unsigned int t = (val->Type & VTK_PARSE_BASE_TYPE);

  t = (t & ~VTK_PARSE_UNSIGNED);
  switch (t)
    {
    case VTK_PARSE_FLOAT:
    case VTK_PARSE_DOUBLE:
    case VTK_PARSE_CHAR:
    case VTK_PARSE_SHORT:
    case VTK_PARSE_INT:
    case VTK_PARSE_LONG:
    case VTK_PARSE_ID_TYPE:
    case VTK_PARSE_LONG_LONG:
    case VTK_PARSE___INT64:
    case VTK_PARSE_SIGNED_CHAR:
    case VTK_PARSE_SSIZE_T:
    case VTK_PARSE_BOOL:
      return 1;
    }

  return 0;
}

int vtkWrap_IsString(ValueInfo *val)
{
  unsigned int t = (val->Type & VTK_PARSE_BASE_TYPE);
  return (t == VTK_PARSE_STRING ||
          t == VTK_PARSE_UNICODE_STRING);
}

/* -------------------------------------------------------------------- */
/* Subcategories */

int vtkWrap_IsBool(ValueInfo *val)
{
  unsigned int t = (val->Type & VTK_PARSE_BASE_TYPE);
  return (t == VTK_PARSE_BOOL);
}

int vtkWrap_IsChar(ValueInfo *val)
{
  unsigned int t = (val->Type & VTK_PARSE_BASE_TYPE);
  return (t == VTK_PARSE_CHAR);
}

int vtkWrap_IsInteger(ValueInfo *val)
{
  unsigned int t = (val->Type & VTK_PARSE_BASE_TYPE);

  t = (t & ~VTK_PARSE_UNSIGNED);
  switch (t)
    {
    case VTK_PARSE_SHORT:
    case VTK_PARSE_INT:
    case VTK_PARSE_LONG:
    case VTK_PARSE_ID_TYPE:
    case VTK_PARSE_LONG_LONG:
    case VTK_PARSE___INT64:
    case VTK_PARSE_SIGNED_CHAR:
    case VTK_PARSE_SSIZE_T:
      return 1;
    }

  return 0;
}

int vtkWrap_IsRealNumber(ValueInfo *val)
{
  unsigned int t = (val->Type & VTK_PARSE_BASE_TYPE);
  return (t == VTK_PARSE_FLOAT || t == VTK_PARSE_DOUBLE);
}

/* -------------------------------------------------------------------- */
/* These are mutually exclusive, as well. */

int vtkWrap_IsScalar(ValueInfo *val)
{
  unsigned int i = (val->Type & VTK_PARSE_POINTER_MASK);
  return (i == 0);
}

int vtkWrap_IsPointer(ValueInfo *val)
{
  unsigned int i = (val->Type & VTK_PARSE_POINTER_MASK);
  return (i == VTK_PARSE_POINTER && val->Count == 0 &&
          val->CountHint == 0 && val->NumberOfDimensions <= 1);
}

int vtkWrap_IsArray(ValueInfo *val)
{
  unsigned int i = (val->Type & VTK_PARSE_POINTER_MASK);
  return (i == VTK_PARSE_POINTER && val->NumberOfDimensions <= 1 &&
          (val->Count != 0 || val->CountHint != 0));
}

int vtkWrap_IsNArray(ValueInfo *val)
{
  int j = 0;
  unsigned int i = (val->Type & VTK_PARSE_POINTER_MASK);
  if (i != VTK_PARSE_ARRAY || val->NumberOfDimensions <= 1)
    {
    return 0;
    }
  for (j = 0; j < val->NumberOfDimensions; j++)
    {
    if (val->Dimensions[j] == NULL || val->Dimensions[j][0] == '\0')
      {
      return 0;
      }
    }
  return 1;
}


/* -------------------------------------------------------------------- */
/* Other type properties, not mutually exclusive. */

int vtkWrap_IsNonConstRef(ValueInfo *val)
{
  return ((val->Type & VTK_PARSE_REF) != 0 &&
          (val->Type & VTK_PARSE_CONST) == 0);
}

int vtkWrap_IsConstRef(ValueInfo *val)
{
  return ((val->Type & VTK_PARSE_REF) != 0 &&
          (val->Type & VTK_PARSE_CONST) != 0);
}

int vtkWrap_IsRef(ValueInfo *val)
{
  return ((val->Type & VTK_PARSE_REF) != 0);
}

int vtkWrap_IsConst(ValueInfo *val)
{
  return ((val->Type & VTK_PARSE_CONST) != 0);
}


/* -------------------------------------------------------------------- */
/* Constructor/Destructor checks */

int vtkWrap_IsConstructor(ClassInfo *c, FunctionInfo *f)

{
  if (c->Name && f->Name &&
      !vtkWrap_IsDestructor(c, f))
    {
    return (strcmp(c->Name, f->Name) == 0);
    }

  return 0;
}

int vtkWrap_IsDestructor(ClassInfo *c, FunctionInfo *f)
{
  size_t i;
  const char *cp;

  if (c->Name && f->Name)
    {
    cp = f->Signature;
    for (i = 0; cp[i] != '\0' && cp[i] != '('; i++)
      {
      if (cp[i] == '~')
        {
        return 1;
        }
      }
    }

  return 0;
}

int vtkWrap_IsSetVectorMethod(FunctionInfo *f)
{
  if (f->Macro && strncmp(f->Macro, "vtkSetVector", 12) == 0)
    {
    return 1;
    }

  return 0;
}

int vtkWrap_IsGetVectorMethod(FunctionInfo *f)
{
  if (f->Macro && strncmp(f->Macro, "vtkGetVector", 12) == 0)
    {
    return 1;
    }

  return 0;
}

/* -------------------------------------------------------------------- */
/* Argument counting */

int vtkWrap_CountWrappedArgs(FunctionInfo *f)
{
  int totalArgs = f->NumberOfArguments;

  if (totalArgs > 0 &&
      (f->Arguments[0]->Type & VTK_PARSE_BASE_TYPE)
       == VTK_PARSE_FUNCTION)
    {
    totalArgs = 1;
    }
  else if (totalArgs == 1 &&
           (f->Arguments[0]->Type & VTK_PARSE_UNQUALIFIED_TYPE)
            == VTK_PARSE_VOID)
    {
    totalArgs = 0;
    }

  return totalArgs;
}

int vtkWrap_CountRequiredArgs(FunctionInfo *f)
{
  int requiredArgs = 0;
  int totalArgs;
  int i;

  totalArgs = vtkWrap_CountWrappedArgs(f);

  for (i = 0; i < totalArgs; i++)
    {
    if (f->Arguments[i]->Value == NULL ||
        vtkWrap_IsArray(f->Arguments[i]) ||
        vtkWrap_IsNArray(f->Arguments[i]))
      {
      requiredArgs = i+1;
      }
    }

  return requiredArgs;
}

/* -------------------------------------------------------------------- */
/* Check whether the class is derived from vtkObjectBase. */

int vtkWrap_IsVTKObjectBaseType(
  HierarchyInfo *hinfo, const char *classname)
{
  HierarchyEntry *entry;

  if (hinfo)
    {
    entry = vtkParseHierarchy_FindEntry(hinfo, classname);
    if (entry)
      {
      if (vtkParseHierarchy_IsTypeOf(hinfo, entry, "vtkObjectBase"))
        {
        return 1;
        }
      return 0;
      }
    }

  /* fallback if no HierarchyInfo */
  if (strncmp("vtk", classname, 3) == 0)
    {
    return 1;
    }

  return 0;
}

/* -------------------------------------------------------------------- */
/* Check if the WRAP_SPECIAL flag is set for the class. */

int vtkWrap_IsSpecialType(
  HierarchyInfo *hinfo, const char *classname)
{
  HierarchyEntry *entry;

  if (hinfo)
    {
    entry = vtkParseHierarchy_FindEntry(hinfo, classname);
    if (vtkParseHierarchy_GetProperty(entry, "WRAP_SPECIAL"))
      {
      return 1;
      }
    return 0;
    }

  /* fallback if no HierarchyInfo */
  if (strncmp("vtk", classname, 3) == 0)
    {
    return -1;
    }

  return 0;
}

/* -------------------------------------------------------------------- */
/* Check if the class is derived from superclass */

int vtkWrap_IsTypeOf(
  HierarchyInfo *hinfo, const char *classname, const char *superclass)
{
  HierarchyEntry *entry;

  if (strcmp(classname, superclass) == 0)
    {
    return 1;
    }

  if (hinfo)
    {
    entry = vtkParseHierarchy_FindEntry(hinfo, classname);
    if (entry && vtkParseHierarchy_IsTypeOf(hinfo, entry, superclass))
      {
      return 1;
      }
    }

  return 0;
}

/* -------------------------------------------------------------------- */
/* Make a guess about whether a class is wrapped */

int vtkWrap_IsClassWrapped(
  HierarchyInfo *hinfo, const char *classname)
{
  if (hinfo)
    {
    HierarchyEntry *entry;
    entry = vtkParseHierarchy_FindEntry(hinfo, classname);

    if (entry)
      {
      if (!vtkParseHierarchy_GetProperty(entry, "WRAP_EXCLUDE") ||
          vtkParseHierarchy_GetProperty(entry, "WRAP_SPECIAL"))
        {
        return 1;
        }
      }
    }
  else if (strncmp("vtk", classname, 3) == 0)
    {
    return 1;
    }

  return 0;
}

/* -------------------------------------------------------------------- */
/* This sets the CountHint for vtkDataArray methods where the
 * tuple size is equal to GetNumberOfComponents. */
void vtkWrap_FindCountHints(
  ClassInfo *data, HierarchyInfo *hinfo)
{
  int i;
  const char *countMethod = "GetNumberOfComponents()";
  FunctionInfo *theFunc;

  for (i = 0; i < data->NumberOfFunctions; i++)
    {
    theFunc = data->Functions[i];

    /* add hints for array GetTuple methods */
    if (vtkWrap_IsTypeOf(hinfo, data->Name, "vtkDataArray"))
      {
      if ((strcmp(theFunc->Name, "GetTuple") == 0 ||
           strcmp(theFunc->Name, "GetTupleValue") == 0) &&
          theFunc->ReturnValue && theFunc->ReturnValue->Count == 0 &&
          theFunc->NumberOfArguments == 1 &&
          theFunc->Arguments[0]->Type == VTK_PARSE_ID_TYPE)
        {
        theFunc->ReturnValue->CountHint = countMethod;
        }
      else if ((strcmp(theFunc->Name, "SetTuple") == 0 ||
                strcmp(theFunc->Name, "SetTupleValue") == 0 ||
                strcmp(theFunc->Name, "GetTuple") == 0 ||
                strcmp(theFunc->Name, "GetTupleValue") == 0 ||
                strcmp(theFunc->Name, "InsertTuple") == 0 ||
                strcmp(theFunc->Name, "InsertTupleValue") == 0) &&
               theFunc->NumberOfArguments == 2 &&
               theFunc->Arguments[0]->Type == VTK_PARSE_ID_TYPE &&
               theFunc->Arguments[1]->Count == 0)
        {
        theFunc->Arguments[1]->CountHint = countMethod;
        }
      else if ((strcmp(theFunc->Name, "InsertNextTuple") == 0 ||
                strcmp(theFunc->Name, "InsertNextTupleValue") == 0) &&
               theFunc->NumberOfArguments == 1 &&
               theFunc->Arguments[0]->Count == 0)
        {
        theFunc->Arguments[0]->CountHint = countMethod;
        }
      }
    }
}


/* -------------------------------------------------------------------- */
/* Expand all typedef types that are used in function arguments */
void vtkWrap_ExpandTypedefs(ClassInfo *data, HierarchyInfo *hinfo)
{
  int i, j, n;
  FunctionInfo *funcInfo;

  n = data->NumberOfFunctions;
  for (i = 0; i < n; i++)
    {
    funcInfo = data->Functions[i];
    if (funcInfo->Access == VTK_ACCESS_PUBLIC)
      {
      for (j = 0; j < funcInfo->NumberOfArguments; j++)
        {
        vtkParseHierarchy_ExpandTypedefs(
          hinfo, funcInfo->Arguments[j], data->Name);
        }
      if (funcInfo->ReturnValue)
        {
        vtkParseHierarchy_ExpandTypedefs(
          hinfo, funcInfo->ReturnValue, data->Name);
        }
      }
    }
}


/* -------------------------------------------------------------------- */
/* get the type name */

const char *vtkWrap_GetTypeName(ValueInfo *val)
{
  unsigned int aType = val->Type;
  const char *aClass = val->Class;

  /* print the type itself */
  switch (aType & VTK_PARSE_BASE_TYPE)
    {
    case VTK_PARSE_FLOAT:          return "float";
    case VTK_PARSE_DOUBLE:         return "double";
    case VTK_PARSE_INT:            return "int";
    case VTK_PARSE_SHORT:          return "short";
    case VTK_PARSE_LONG:           return "long";
    case VTK_PARSE_VOID:           return "void ";
    case VTK_PARSE_CHAR:           return "char";
    case VTK_PARSE_UNSIGNED_INT:   return "unsigned int";
    case VTK_PARSE_UNSIGNED_SHORT: return "unsigned short";
    case VTK_PARSE_UNSIGNED_LONG:  return "unsigned long";
    case VTK_PARSE_UNSIGNED_CHAR:  return "unsigned char";
    case VTK_PARSE_ID_TYPE:        return "vtkIdType";
    case VTK_PARSE_LONG_LONG:      return "long long";
    case VTK_PARSE___INT64:        return "__int64";
    case VTK_PARSE_UNSIGNED_LONG_LONG: return "unsigned long long";
    case VTK_PARSE_UNSIGNED___INT64:   return "unsigned __int64";
    case VTK_PARSE_SIGNED_CHAR:    return "signed char";
    case VTK_PARSE_BOOL:           return "bool";
    case VTK_PARSE_UNICODE_STRING: return "vtkUnicodeString";
    case VTK_PARSE_SSIZE_T:        return "ssize_t";
    case VTK_PARSE_SIZE_T:         return "size_t";
    }

  return aClass;
}

/* -------------------------------------------------------------------- */
/* variable declarations */

void vtkWrap_DeclareVariable(
  FILE *fp, ValueInfo *val, const char *name, int i, int flags)
{
  unsigned int aType;
  const char *aClass;
  int j;

  if (val == NULL)
    {
    return;
    }

  aType = (val->Type & VTK_PARSE_UNQUALIFIED_TYPE);
  aClass = val->Class;

  /* do nothing for void */
  if (aType == VTK_PARSE_VOID ||
      (aType & VTK_PARSE_BASE_TYPE) == VTK_PARSE_FUNCTION)
    {
    return;
    }

  /* add a couple spaces */
  fprintf(fp,"  ");

  /* for const * return types, prepend with const */
  if ((flags & VTK_WRAP_RETURN) != 0)
    {
    if ((val->Type & VTK_PARSE_CONST) != 0 &&
        (aType & VTK_PARSE_INDIRECT) != 0)
      {
      fprintf(fp,"const ");
      }
    }
  /* do the same for "const char *" with initializer */
  else
    {
    if ((val->Type & VTK_PARSE_CONST) != 0 &&
        aType == VTK_PARSE_CHAR_PTR &&
        val->Value &&
        strcmp(val->Value, "0") != 0 &&
        strcmp(val->Value, "NULL") != 0)
      {
      fprintf(fp,"const ");
      }
    }

  /* print the type name */
  fprintf(fp, "%s ", vtkWrap_GetTypeName(val));

  /* indirection */
  if ((flags & VTK_WRAP_RETURN) != 0)
    {
    /* ref and pointer return values are stored as pointers */
    if ((aType & VTK_PARSE_INDIRECT) == VTK_PARSE_POINTER ||
        (aType & VTK_PARSE_INDIRECT) == VTK_PARSE_REF)
      {
      fprintf(fp, "*");
      }
    }
  else
    {
    /* objects refs and pointers are always handled via pointers,
     * other refs are passed by value */
    if (aType == VTK_PARSE_CHAR_PTR ||
        aType == VTK_PARSE_VOID_PTR ||
        aType == VTK_PARSE_OBJECT_PTR ||
        aType == VTK_PARSE_OBJECT_REF ||
        aType == VTK_PARSE_OBJECT ||
        vtkWrap_IsQtObject(val))
      {
      fprintf(fp, "*");
      }
    /* arrays of unknown size are handled via pointers */
    else if (val->CountHint)
      {
      fprintf(fp, "*");
      }
    }

  /* the variable name */
  if (i >= 0)
    {
    fprintf(fp,"%s%i", name, i);
    }
  else
    {
    fprintf(fp,"%s", name);
    }

  if ((flags & VTK_WRAP_ARG) != 0)
    {
    /* print the array decorators */
    if (((aType & VTK_PARSE_POINTER_MASK) != 0) &&
        aType != VTK_PARSE_CHAR_PTR &&
        aType != VTK_PARSE_VOID_PTR &&
        aType != VTK_PARSE_OBJECT_PTR &&
        !vtkWrap_IsQtObject(val) &&
        val->CountHint == NULL)
      {
      if (val->NumberOfDimensions == 1 && val->Count > 0)
        {
        fprintf(fp, "[%d]", val->Count);
        }
      else
        {
        for (j = 0; j < val->NumberOfDimensions; j++)
          {
          fprintf(fp, "[%s]", val->Dimensions[j]);
          }
        }
      }

    /* add a default value */
    else if (val->Value)
      {
      fprintf(fp, " = %s", val->Value);
      }
    else if (aType == VTK_PARSE_CHAR_PTR ||
             aType == VTK_PARSE_VOID_PTR ||
             aType == VTK_PARSE_OBJECT_PTR ||
             aType == VTK_PARSE_OBJECT_REF ||
             aType == VTK_PARSE_OBJECT ||
             vtkWrap_IsQtObject(val))
      {
      fprintf(fp, " = NULL");
      }
    else if (val->CountHint)
      {
      fprintf(fp, " = NULL");
      }
    else if (aType == VTK_PARSE_BOOL)
      {
      fprintf(fp, " = false");
      }
    }

  /* finish off with a semicolon */
  fprintf(fp, ";\n");
}

void vtkWrap_DeclareVariableSize(
  FILE *fp, ValueInfo *val, const char *name, int i)
{
  char idx[32];
  int j;

  idx[0] = '\0';
  if (i >= 0)
    {
    sprintf(idx, "%d", i);
    }

  if (val->NumberOfDimensions > 1)
    {
    fprintf(fp,
            "  static int %s%s[%d] = ",
            name, idx, val->NumberOfDimensions);

    for (j = 0; j < val->NumberOfDimensions; j++)
      {
      fprintf(fp, "%c %s", ((j == 0) ? '{' : ','), val->Dimensions[j]);
      }

    fprintf(fp, " };\n");
    }
  else if (val->Count != 0 || val->CountHint)
    {
    fprintf(fp,
            "  %sint %s%s = %d;\n",
            (val->CountHint ? "" : "const "), name, idx,
            (val->CountHint ? 0 : val->Count));
    }
  else if (val->NumberOfDimensions == 1)
    {
    fprintf(fp,
            "  const int %s%s = %s;\n",
            name, idx, val->Dimensions[0]);
    }
}
