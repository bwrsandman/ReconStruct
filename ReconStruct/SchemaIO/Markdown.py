# -*- coding: utf-8 -*-
"""
ReconStruct

Reconstruct is a application which helps easily reverse engineer binary file
formats.  It is tested to run on Python 2.7, 3.3, 3.4 and pypy.

Copyright (c) 2014 Sandy Carter
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
"""

from markdownwriter import MarkdownWriter, MarkdownTable
from markdown import Markdown, util

try:
    from ReconStruct.ManifestMain import ManifestMain
except ImportError:
    from ManifestMain import ManifestMain


# Not to be translated
headers = ["Label", "Size", "Type"]
attr_headers = ["Attribute", "Value"]


def toMarkdown(manifest):
    writer = MarkdownWriter()
    writer.addHeader(manifest.type_name)
    table = MarkdownTable(headers)
    for sub_manifest in manifest.sub_manifests:
        table.addRow([
            sub_manifest.label,
            str(sub_manifest._size),
            sub_manifest.type(),
        ])
    if not manifest.sub_manifests:
        table.addRow([
            "",
            "0",
            "bytes",
        ])
    writer.addTable(table)
    writer.addSimpleLineBreak()
    for type_name, custom_manifest in manifest.saved_manifests.iteritems():
        writer.addHeader(type_name, level=2)
        table = MarkdownTable(headers)
        for sub_manifest in custom_manifest.sub_manifests:
            table.addRow([
                sub_manifest.label,
                str(sub_manifest._size),
                sub_manifest.type(),
            ])
        if not custom_manifest.sub_manifests:
            table.addRow([
                "",
                "0",
                "bytes",
            ])
        writer.addTable(table)
        writer.addSimpleLineBreak()
    attrs = manifest.getFileAttributes()
    if attrs:
        table = MarkdownTable(attr_headers)
        for key, val in attrs.iteritems():
            table.addRow([key, val])
        writer.addHeader('Attributes')
        writer.addTable(table)
    return writer.getStream()


def fromMarkdown(source, app):

    def _getElemFromSource(source):
        md = Markdown(extensions=['tables'])
        if not source.strip():
            raise ValueError(app.tr(
                "Schema file is empty."
            ))
        try:
            source = util.text_type(source)
        except UnicodeDecodeError as e:
            e.reason += '. -- Note: Markdown only accepts unicode input!'
            raise
        lines = source.split("\n")
        for prep in md.preprocessors.values():
            lines = prep.run(lines)
        # Parse the high-level elements.
        return md.parser.parseDocument(lines).getroot()

    def readTable(table, headers):
        if len(table) != 2:
            raise ValueError(app.tr(
                "Type table does not have two expected sub elements."
            ))
        thead, tbody = table
        if thead.tag != 'thead' or tbody.tag != 'tbody':
            raise ValueError(app.tr(
                "Type table does not have correct tags for sub elements. "
                "Expected ('thead', 'tbody'), got ('%s', '%s')." %
                (thead.tag, tbody.tag)
            ))
        table_headers = [t.text for t in thead[0]]
        if table_headers != headers:
            raise ValueError(app.tr(
                "Type table does not have the correct headers. "
                "Expected %s, got %s." % (headers, table_headers)
            ))
        for line in tbody:
            if len(line) != len(headers):
                raise ValueError(app.tr(
                    "Row of type table has incorrect number of columns. "
                    "Expected %d, got %d." % (len(headers), len(line))
                ))
        return ((j.text.strip() for j in row) for row in tbody)

    def _readAttributesTable(table):
        return dict(readTable(table, attr_headers))

    def _readManifestTable(main_manifest, table, data_type='main'):
        if data_type == 'main':
            parent_manifest = main_manifest
        else:
            if data_type not in main_manifest.saved_manifests:
                main_manifest.add_custom_type(data_type)
            parent_manifest = main_manifest.saved_manifests[data_type]
        for label, size, data_type in readTable(table, headers):
            if not size:
                raise ValueError(app.tr("Row does not have a value for size."))
            if not data_type:
                raise ValueError(app.tr("Row does not have a value for type."))
            manifest = main_manifest.get_manifest(data_type)
            parent_manifest.add(manifest(label, size, data_type))
            continue
    root = _getElemFromSource(source)
    main_manifest = ManifestMain()
    if len(root) < 2:
        raise ValueError(app.tr("Schema does not have data type information."))
    main_h1 = root[0]
    if main_h1.tag != "h1":
        raise ValueError(app.tr("Schema is missing main title."))
    if main_h1.text != "main":
        raise ValueError(app.tr(
            "Schema main title is mislabeled. Expected 'main', got '%s'."
            % main_h1.text
        ))
    main_table = root[1]
    if main_table.tag != "table":
        raise ValueError(app.tr("Schema main data table is missing."))
    _readManifestTable(main_manifest, main_table)

    if len(root) % 2 != 0:
        raise ValueError(app.tr(
            "Schema is missing data type information for custom types."
        ))

    # Attributes appended at the end
    end_index = len(root)
    if (len(root) >= 2 and root[-2].tag == 'h1'
            and root[-2].text == 'Attributes'):
        end_index -= 2
        main_manifest.setFileAttributes(_readAttributesTable(root[-1]))

    for i in range(2, end_index, 2):
        sub_h2 = root[i]
        sub_table = root[i + 1]
        if sub_h2.tag != "h2":
            raise ValueError(app.tr("Schema is missing custom data type title."))
        type_name = sub_h2.text.strip()
        if sub_table.tag != "table":
            raise ValueError(app.tr(
                "Schema custom data table is missing for %s." % type_name
            ))
        _readManifestTable(main_manifest, sub_table, type_name)
    return main_manifest
