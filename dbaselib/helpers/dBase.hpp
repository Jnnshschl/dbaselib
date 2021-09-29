#pragma once

#include <string>
#include <vector>
#include <filesystem>

/// <summary>
/// Main interface to the DBASE data fields.
/// </summary>
class DBaseHandle
{
public:
    /// <summary>
    /// Returns the name of the DBASE field.
    /// </summary>
    virtual const char* Name() const noexcept = 0;

    /// <summary>
    /// Returns the max size of the DBASE field.
    /// </summary>
    virtual size_t Size() const noexcept = 0;

    /// <summary>
    /// Returns the amount of decimals of the DBASE field.
    /// </summary>
    virtual size_t Decimals() const noexcept = 0;

    /// <summary>
    /// Returns the type of the DBASE field.
    /// </summary>
    virtual char Type() const noexcept = 0;

    /// <summary>
    /// Returns a pointer to the given rows raw data.
    /// </summary>
    /// <param name="row">Row to get the pointer of.</param>
    virtual char* Data(int row) const noexcept = 0;

    /// <summary>
    /// Copy another fields data to this field.
    /// </summary>
    /// <param name="row">Target row id.</param>
    /// <param name="src">Source handle.</param>
    /// <param name="srcRow">Source row id.</param>
    virtual void Copy(int row, const DBaseHandle* src, int srcRow) const noexcept = 0;

    /// <summary>
    /// Copy bytes to the field.
    /// </summary>
    /// <param name="row">Target row id.</param>
    /// <param name="src">Pointer to the data to copy.</param>
    /// <param name="size">Size of the data to copy.</param>
    virtual void CopyRaw(int row, const void* src, size_t size) const noexcept = 0;

    /// <summary>
    /// Insert a text into a field.
    /// </summary>
    /// <param name="row">Target row id.</param>
    /// <param name="offset">Offset.</param>
    /// <param name="src">Text to insert.</param>
    /// <param name="size">Size fo the text to insert.</param>
    virtual void Insert(int row, int offset, const void* src, size_t size) const noexcept = 0;

    /// <summary>
    /// Copy another fields data to this field in right padded mode, use this for numbers/dates.
    /// </summary>
    /// <param name="row">Target row id.</param>
    /// <param name="src">Source handle.</param>
    /// <param name="srcRow">Source row id.</param>
    virtual void CopyR(int row, const DBaseHandle* other, int otherRow) const noexcept = 0;

    /// <summary>
    /// Copy bytes to the field in right padded mode, use this for numbers/dates.
    /// </summary>
    /// <param name="row">Target row id.</param>
    /// <param name="src">Pointer to the data to copy.</param>
    /// <param name="size">Size of the data to copy.</param>
    virtual void CopyRRaw(int row, const void* src, size_t size) const noexcept = 0;

    /// <summary>
    /// Clears the field with ' ' (spaces).
    /// </summary>
    /// <param name="row">Row id.</param>
    virtual void Clear(int row) const noexcept = 0;

    /// <summary>
    /// Returns the given field as a string_view (Text).
    /// </summary>
    /// <param name="row">Row id.</param>
    virtual std::string_view GetText(int row) const noexcept = 0;

    /// <summary>
    /// Set the value of the field to the given text. Caution: strlen()
    /// will be used to measure the strings size! This may lead to
    /// problems because dbase doesnt terminate strings, use the copy
    /// function instead.
    /// </summary>
    /// <param name="row">Row id.</param>
    /// <param name="text">Value to set.</param>
    virtual void SetText(int row, const char* text) const noexcept = 0;

    /// <summary>
    /// Set the value of the field to the given text.
    /// </summary>
    /// <param name="row">Row id.</param>
    /// <param name="text">Value to set.</param>
    virtual void SetText(int row, const std::string& text) const noexcept = 0;

    /// <summary>
    /// Set the value of the field to the given text.
    /// </summary>
    /// <param name="row">Row id.</param>
    /// <param name="text">Value to set.</param>
    virtual void SetText(int row, const std::string_view& text) const noexcept = 0;

    /// <summary>
    /// Set the value of the field to the given char.
    /// </summary>
    /// <param name="row">Row id.</param>
    /// <param name="c">Value to set.</param>
    virtual void SetChar(int row, char c) const noexcept = 0;

    /// <summary>
    /// Replace text in the fields text.
    /// </summary>
    /// <param name="row">Row id.</param>
    /// <param name="text">Text to replace.</param>
    /// <param name="newText">Text to insert.</param>
    virtual void ReplaceText(int row, const char* text, const char* newText) const noexcept = 0;

    /// <summary>
    /// Returns the given field as a float.
    /// </summary>
    /// <param name="row">Row id.</param>
    virtual float GetFloat(int row) const noexcept = 0;

    /// <summary>
    /// Set the value of the field to the given float. The float
    /// will be rounded to the fields decimal count.
    /// </summary>
    /// <param name="row">Row id.</param>
    /// <param name="f">Value to set.</param>
    virtual void SetFloat(int row, float f) const noexcept = 0;

    /// <summary>
    /// Returns the given field as a int.
    /// </summary>
    /// <param name="row">Row id.</param>
    virtual int GetInt(int row) const noexcept = 0;

    /// <summary>
    /// Set the value of the field to the given int.
    /// </summary>
    /// <param name="row">Row id.</param>
    /// <param name="i">Value to set.</param>
    virtual void SetInt(int row, int i) const noexcept = 0;

    /// <summary>
    /// Set the value of the field to the given date.
    /// </summary>
    /// <param name="row">Row id.</param>
    /// <param name="t">Date to set.</param>
    virtual void SetDate(int row, tm t) const noexcept = 0;

    /// <summary>
    /// Set the value of the field to the given date.
    /// </summary>
    /// <param name="row">Row id.</param>
    /// <param name="d">Day to set.</param>
    /// <param name="m">Month to set.</param>
    /// <param name="y">Year to set.</param>
    virtual void SetDate(int row, int d, int m, int y) const noexcept = 0;
};

/// <summary>
/// Class to manage DBASE structure.
/// </summary>
class DBase
{
public:
    const char* Data;
    const size_t Size;
    const bool ClaimData;

    std::vector<char*> Records;
    std::vector<char*> Deleted;
    std::vector<std::string> FieldNames;

    DBase(char* data, size_t size, bool claimData = true)
        : Data(data),
        Size(size),
        Records(),
        Deleted(),
        ClaimData(claimData)
    {}

    ~DBase()
    {
        if (ClaimData) delete[] Data;
    }

    /// <summary>
    /// Returns the total record count of the DBASE file.
    /// Ignores deleted entries.
    /// </summary>
    constexpr auto RecordCount() const noexcept { return Records.size(); }

    /// <summary>
    /// Returns all the available field names of the DBASE file.
    /// </summary>
    constexpr const auto& Fields() const noexcept { return FieldNames; }

    /// <summary>
    /// Returns the DBASE version.
    /// </summary>
    virtual char Version() const noexcept = 0;

    /// <summary>
    /// Load the DBASE data.
    /// </summary>
    /// <returns>True if the loaded successfully, false if not.</returns>
    virtual bool Load() noexcept = 0;

    /// <summary>
    /// Save the DBASE as a file.
    /// </summary>
    /// <param name="file">File to save it to.</param>
    virtual void Save(std::filesystem::path file) const noexcept = 0;

    /// <summary>
    /// Select a field to obtain a handle for it. The
    /// handle can then be used to edit the data.
    /// </summary>
    /// <param name="col">Name of the column (case sensitive).</param>
    /// <returns>Handle for the given column.</returns>
    virtual DBaseHandle* Select(const std::string& col) const noexcept = 0;
};