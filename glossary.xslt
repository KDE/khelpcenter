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
	<xsl:apply-templates select="glossentry"/>
</section>
</xsl:template>

<xsl:template match="glossentry">
<entry>
	<term><xsl:value-of select="glossterm"/></term>
	<definition><xsl:value-of select="glossdef/*[not(name()='glossseealso')]"/></definition>
	<references><xsl:apply-templates select="glossdef/glossseealso"/></references>
</entry>
</xsl:template>

<xsl:template match="glossseealso">
<reference>
	<term><xsl:value-of select="."/></term>
</reference>
</xsl:template>

</xsl:stylesheet>
