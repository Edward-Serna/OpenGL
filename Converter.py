import os
import xml.etree.ElementTree as ET

def process_vcxproj_files(root_folder):
    for foldername, _, filenames in os.walk(root_folder):
        for filename in filenames:
            if filename.endswith('.vcxproj'):
                file_path = os.path.join(foldername, filename)
                update_vcxproj(file_path)

def update_vcxproj(file_path):
    try:
        # Parse the XML file
        tree = ET.parse(file_path)
        root = tree.getroot()

        # Set the correct namespace and namespace URL
        namespace_url = 'http://schemas.microsoft.com/developer/msbuild/2003'
        ET.register_namespace('', namespace_url)

        # Update the Project element with the correct namespace
        root.tag = f'{{{namespace_url}}}Project'

        # Find and update IncludePath and LibraryPath elements in PropertyGroup
        for prop_group in root.findall('./ns:PropertyGroup', {'ns': namespace_url}):
            condition = prop_group.get('Condition')
            if condition and ('Debug|Win32' in condition or 'Debug|x64' in condition):
                include_path_elem = prop_group.find('ns:IncludePath', {'ns': namespace_url})
                if include_path_elem is not None:
                    """
                    CHANGE TO YOUR INCLUDE PATH
                    """
                    include_path_elem.text = r'C:\Users\usl\Documents\OpenGL\ImGUI GLFW Tutorial\Libraries\include' 

                library_path_elem = prop_group.find('ns:LibraryPath', {'ns': namespace_url})
                if library_path_elem is not None:
                    """
                    CHANGE TO YOUR LIBRARY PATH
                    """
                    library_path_elem.text = r'C:\Users\usl\Documents\OpenGL\ImGUI GLFW Tutorial\Libraries\lib'

        # Save the modified XML back to the file
        tree.write(file_path, encoding='utf-8', xml_declaration=True)

        print(f"Successfully updated {file_path}")

    except Exception as e:
        print(f"Error processing file {file_path}: {e}")

if __name__ == '__main__':
    """
    CHANGE TO YOUR DOWNLOAD/MOVED-TO PATH
    """
    root_folder = r'C:\Users\usl\Documents\OpenGL'

    process_vcxproj_files(root_folder)
