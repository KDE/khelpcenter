<?xml version="1.0"?>
<xsl:stylesheet xmlns:xsl="http://www.w3.org/1999/XSL/Transform"
                version="1.0">

<xsl:template match="glossary">
<glossary>
<xsl:apply-templates select="glossdiv"/>
</glossary>
</xsl:template>

<xsl:template match="glossdiv">
<section>
<title><xsl:value-of select="title"/></title>
<anchor><xsl:value-of select="@id"/></anchor>
<xsl:apply-templates/>
</section>
</xsl:template>

<xsl:template match="glossentry">
<entry>
<term><xsl:value-of select="glossterm"/></term>
<definition><xsl:value-of select="glossdef/*[not(name()='glossseealso')]"/></definition>
<xsl:apply-templates select="glossdef/glossseealso"/>
</entry>
</xsl:template>

<xsl:template match="glossseealso">
<reference>
<term><xsl:value-of select="."/></term>
<anchor><xsl:value-of select="@otherterm"/></anchor>
</reference>
</xsl:template>

</xsl:stylesheet>
