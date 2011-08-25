/**************************************************************************
 *  NetBox plugin for FAR 2.0 (http://code.google.com/p/farplugs)         *
 *  Copyright (C) 2011 by Artem Senichev <artemsen@gmail.com>             *
 *  Copyright (C) 2011 by Michael Lukashov <michael.lukashov@gmail.com>   *
 *                                                                        *
 *  This program is free software: you can redistribute it and/or modify  *
 *  it under the terms of the GNU General Public License as published by  *
 *  the Free Software Foundation, either version 3 of the License, or     *
 *  (at your option) any later version.                                   *
 *                                                                        *
 *  This program is distributed in the hope that it will be useful,       *
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *  GNU General Public License for more details.                          *
 *                                                                        *
 *  You should have received a copy of the GNU General Public License     *
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>. *
 **************************************************************************/

#pragma once

#include "Protocol.h"
#include "Session.h"


/**
 * Saved sessions manager
 */
class CSessionManager : public IProtocol
{
public:
    //From IProtocol
    virtual bool Connect(HANDLE, std::wstring &)
    {
        assert(NULL);
        return false;
    }
    virtual void Close()                    {}
    virtual bool CheckExisting(const wchar_t *path, const ItemType type, bool &isExist, std::wstring &errorInfo);
    virtual bool ChangeDirectory(const wchar_t *name, std::wstring &errorInfo);
    virtual const wchar_t *GetCurrentDirectory()
    {
        return m_CurrentDirectory.c_str();
    }
    virtual bool MakeDirectory(const wchar_t *path, std::wstring &errorInfo);
    virtual bool GetList(PluginPanelItem **items, int *itemsNum, std::wstring &errorInfo);
    virtual void FreeList(PluginPanelItem *items, int itemsNum);
    virtual bool GetFile(const wchar_t *, const wchar_t *, const unsigned __int64, std::wstring &)
    {
        assert(NULL);
        return false;
    }
    virtual bool PutFile(const wchar_t *remotePath, const wchar_t *localPath, const unsigned __int64 fileSize, std::wstring &errorInfo);
    virtual bool Rename(const wchar_t *, const wchar_t *, const ItemType, std::wstring &)
    {
        assert(NULL);
        return false;
    }
    virtual bool Delete(const wchar_t *path, const ItemType type, std::wstring &errorInfo);
    virtual std::wstring GetURL(const bool)
    {
        return std::wstring();
    }
    virtual int GetProgress()
    {
        return -1;
    }

    virtual bool TryToResolveConnectionProblem()
    {
        return false;
    }

    virtual bool Aborted() const
    {
        return false;
    }

public:
    /**
     * Get open plugin (panel) info
     * \param pluginInfo pointer to info structure
     */
    void GetOpenPluginInfo(OpenPluginInfo *pluginInfo);

    /**
     * Process key press event
     * \param key key code
     * \param controlState keyboard control state
     * \return FAR's return code for ProcessKeyW
     */
    int ProcessKey(HANDLE plugin, const int key, const unsigned int controlState);

    /**
     * Create session from existing file
     * \param name file name
     * \return session pointer (NULL if error)
     */
    PSession LoadSession(const wchar_t *name);

private:
    /**
     * Convert path to windows format
     * \param pathBase base path
     * \param sub additional path
     * \return result path
     */
    std::wstring ConvertPath(const wchar_t *pathBase, const wchar_t *sub = NULL) const;

private:
    std::wstring m_CurrentDirectory;  ///< Current directory name
};
