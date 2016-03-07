#include "ProjectManager.hpp"
#include "ViewManager.hpp"
#include "../DataSpecRegistry.hpp"

namespace urde
{
static logvisor::Module Log("URDE::ProjectManager");

void ProjectManager::IndexMP1Resources()
{
    const std::vector<hecl::Database::Project::ProjectDataSpec>& specs = m_proj->getDataSpecs();
    for (const hecl::Database::Project::ProjectDataSpec& spec : m_proj->getDataSpecs())
    {
        if (&spec.spec == &DataSpec::SpecEntMP1)
        {
            m_factory.BuildObjectMap(spec);
            break;
        }
    }
}

bool ProjectManager::m_registeredSpecs = false;
ProjectManager::ProjectManager(ViewManager &vm)
: m_vm(vm), m_objStore(m_factory)
{
    if (!m_registeredSpecs)
    {
        HECLRegisterDataSpecs();
        m_registeredSpecs = true;
    }
}

bool ProjectManager::newProject(const hecl::SystemString& path)
{
    hecl::ProjectRootPath projPath = hecl::SearchForProject(path);
    if (projPath)
    {
        Log.report(logvisor::Warning, _S("project already exists at '%s'"), path.c_str());
        return false;
    }

    hecl::MakeDir(path.c_str());
    m_proj.reset(new hecl::Database::Project(path));
    if (!*m_proj)
    {
        m_proj.reset();
        return false;
    }

    m_vm.ProjectChanged(*m_proj);
    m_vm.SetupEditorView();
    saveProject();

    hecl::SystemString windowTitle(m_proj->getProjectRootPath().getLastComponent());
    windowTitle += _S(" - URDE");
    m_vm.m_mainWindow->setTitle(windowTitle.c_str());
    m_vm.DismissSplash();
    m_vm.FadeInEditors();

    return true;
}

bool ProjectManager::openProject(const hecl::SystemString& path)
{
    hecl::ProjectRootPath projPath = hecl::SearchForProject(path);
    if (!projPath)
    {
        Log.report(logvisor::Warning, _S("project doesn't exist at '%s'"), path.c_str());
        return false;
    }

    m_proj.reset(new hecl::Database::Project(projPath));
    if (!*m_proj)
    {
        m_proj.reset();
        return false;
    }

    hecl::ProjectPath urdeSpacesPath(*m_proj, _S(".hecl/urde_spaces.yaml"));
    FILE* fp = hecl::Fopen(urdeSpacesPath.getAbsolutePath().c_str(), _S("r"));

    athena::io::YAMLDocReader r;
    if (!fp)
        goto makeDefault;

    yaml_parser_set_input_file(r.getParser(), fp);
    if (!r.ValidateClassType("UrdeSpacesState"))
    {
        fclose(fp);
        goto makeDefault;
    }

    r.reset();
    fseek(fp, 0, SEEK_SET);
    yaml_parser_set_input_file(r.getParser(), fp);
    if (!r.parse())
    {
        fclose(fp);
        goto makeDefault;
    }
    fclose(fp);

    m_vm.ProjectChanged(*m_proj);
    m_vm.SetupEditorView(r);

    IndexMP1Resources();
    m_vm.BuildTestPART(m_objStore);

    {
        hecl::SystemString windowTitle(m_proj->getProjectRootPath().getLastComponent());
        windowTitle += _S(" - URDE");
        m_vm.m_mainWindow->setTitle(windowTitle.c_str());
    }
    m_vm.DismissSplash();
    m_vm.FadeInEditors();

    m_vm.pushRecentProject(m_proj->getProjectRootPath().getAbsolutePath());

    return true;

makeDefault:
    m_vm.ProjectChanged(*m_proj);
    m_vm.SetupEditorView();
    saveProject();

    {
        hecl::SystemString windowTitle(m_proj->getProjectRootPath().getLastComponent());
        windowTitle += _S(" - URDE");
        m_vm.m_mainWindow->setTitle(windowTitle.c_str());
    }
    m_vm.DismissSplash();
    m_vm.FadeInEditors();
    return true;
}

bool ProjectManager::extractGame(const hecl::SystemString& path)
{
    return false;
}

bool ProjectManager::saveProject()
{
    if (!m_proj)
        return false;

    hecl::ProjectPath oldSpacesPath(*m_proj, _S(".hecl/~urde_spaces.yaml"));
    FILE* fp = hecl::Fopen(oldSpacesPath.getAbsolutePath().c_str(), _S("w"));
    if (!fp)
        return false;

    athena::io::YAMLDocWriter w("UrdeSpacesState");
    yaml_emitter_set_output_file(w.getEmitter(), fp);
    m_vm.SaveEditorView(w);
    if (!w.finish())
    {
        fclose(fp);
        return false;
    }
    fclose(fp);

    hecl::ProjectPath newSpacesPath(*m_proj, _S(".hecl/urde_spaces.yaml"));

    hecl::Unlink(newSpacesPath.getAbsolutePath().c_str());
    hecl::Rename(oldSpacesPath.getAbsolutePath().c_str(),
                 newSpacesPath.getAbsolutePath().c_str());

    m_vm.pushRecentProject(m_proj->getProjectRootPath().getAbsolutePath());

    return true;
}

}
