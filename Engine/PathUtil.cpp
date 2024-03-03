#include "PathUtil.h"
#include "RWindow.h"

std::filesystem::path debugPath;

void PathUtil::SetDebugPath(std::filesystem::path path)
{
    debugPath = std::filesystem::weakly_canonical(path);
}

std::filesystem::path PathUtil::GetModuleFilePath()
{
    wchar_t pathBuf[MAX_PATH];
    GetModuleFileName(RWindow::GetWindowClassEx().hInstance, pathBuf, sizeof(pathBuf) / sizeof(pathBuf[0]));
    return std::filesystem::path(pathBuf);
}

std::filesystem::path PathUtil::ConvertAbsolute(std::filesystem::path path, std::filesystem::path base)
{
    if (path.is_absolute()) {
        return path;
    }

    if (base.empty()) {
        base = GetMainPath();
    }
    else if (!base.is_absolute()) {
        base = GetMainPath() / base;
        base = std::filesystem::weakly_canonical(base);
    }

    base = RemoveFileName(base);

    return std::filesystem::weakly_canonical(base / path);
}

std::filesystem::path PathUtil::ConvertRelativeFromTo(std::filesystem::path a, std::filesystem::path b)
{
    std::filesystem::path from = a;
    if (!a.is_absolute()) {
        from = GetMainPath() / a;
        from = std::filesystem::weakly_canonical(from);
    }

    from = RemoveFileName(from);

    std::filesystem::path to = b;
    if (!b.is_absolute()) {
        to = from / b;
        to = std::filesystem::weakly_canonical(to);
    }

    from = std::filesystem::absolute(from);
    to = std::filesystem::absolute(to);

    return std::filesystem::relative(to, from);
}

std::filesystem::path PathUtil::ConvertRelativeFromMainPath(std::filesystem::path path)
{
    if (!path.is_absolute()) {
        path = GetMainPath() / path;
        path = std::filesystem::weakly_canonical(path);
    }

    return std::filesystem::relative(path, GetMainPath());
}

std::filesystem::path PathUtil::RemoveFileName(std::filesystem::path path)
{
    if (std::filesystem::exists(path)) {
        if (std::filesystem::is_directory(path)) {
            //ディレクトリなので操作しない
            return path;
        }
        //ファイル名なので消してよし
        return path.remove_filename();
    }

    if (path.has_extension()) {
        //拡張子を持っているので消してよし
        return path.remove_filename();
    }
    //拡張子持たないファイルかディレクトリなのでそのまま
    //これ以上は特定できないと思う
    return path;
}

std::filesystem::path PathUtil::GetMainPath()
{
    if (!debugPath.empty()) {
        return debugPath;
    }

    std::filesystem::path path = GetModuleFilePath();
    return RemoveFileName(path);
}
